#include "gemmini.h"

#include <bits/stdc++.h>
#include <ilang/ilang++.h>

namespace gemmini{

    Gemmini::Gemmini(cfg Cfg) 
        :m(Ila("Gemmini")),
        // ---------- Inputs ----------
        funct(m.NewBvInput("funct", 3)),
        rs1(m.NewBvInput("rs1", RS_WIDTH)),
        rs2(m.NewBvInput("rs2", RS_WIDTH)),
        // ---------- Config states ----------
        dataflow(m.NewBoolState("dataflow")),
        shift(m.NewBvState("shift", 32)),
        A_stride(m.NewBvState("A_stride", 16)),
        scale(m.NewBvState("scale", 32)),
        private_stride(m.NewBvState("private_stride", 16)),
        memory_stride_mvin(m.NewBvState("memory_stride_mvin", 64)),
        memory_stride_mvout(m.NewBvState("memory_stride_mvout", 64)),
        right_shift(m.NewBvState("right_shift", 32)),
        activation_func(m.NewBvState("activation_func", 1)),
        A_T(m.NewBvState("A_T", 1)),
        B_T(m.NewBvState("B_T", 1)),
        scalar(m.NewBvState("scalar", 32)),
        acc_type(m.NewBvState("acc_type", 1)),
        mvin_type(m.NewBvState("mvin_type", 2)),
        max_pool_stride(m.NewBvState("max_pool_stride", 2)),
        max_pool_window_size(m.NewBvState("max_pool_window_size", 2)),
        upper_zero_pad(m.NewBvState("upper_zero_pad", 2)),
        left_zero_pad(m.NewBvState("left_zero_pad", 2)),
        out_dim(m.NewBvState("out_dim", 8)),
        pool_row(m.NewBvState("pool_row", 8)),
        pool_col(m.NewBvState("pool_col", 8)),
        unpool_row(m.NewBvState("unpool_row", 8)),
        unpool_col(m.NewBvState("unpool_col", 8)),
        // ---------- Extra for preload ----------
        dest_addr(m.NewBvState("dest_addr", 32)),
        dest_row(m.NewBvState("dest_row", 32)),
        dest_col(m.NewBvState("dest_col", 32)), 
        DRAM(m.NewMemState("DRAM", 64, 32)),
        scratchpad(m.NewMemState("scratchpad", Cfg.DIM * Cfg.input_bits(), 32)),
        accumulator(m.NewMemState("accumulator", Cfg.DIM * Cfg.acc_bits(), 32))
        {
            // ---------- Store config ----------
            _Cfg = Cfg;
            DIM = Cfg.DIM;
            
            // ---------- Create Systolic Array ----------
            sys_array.resize(DIM);
            for (size_t i = 0; i < DIM; i++) {
                sys_array[i].resize(DIM);
                for (size_t j = 0; j < DIM; j++) {
                    sys_array[i][j] = std::make_unique<PE>(m, i, j);
                }
            }
        };
    
    void Gemmini::AddInstructions(){

        {
            // Data Movement
            // rs1 is DRAM address
            {
                // mvin
                InstrRef instr = m.NewInstr("mvin");
                auto decode = mvin;
                instr.SetDecode(decode);
                auto DRAM_addr = Extract(rs1, 63,0);
                auto scratchpad_addr = Extract(rs2, 31, 0);
                auto col_num = Extract(rs2, 47, 32);
                auto row_num = Extract(rs2, 64, 48);
                auto destination = Extract(rs2, 31, 31);
                auto chunks = (col_num + BvConst(DIM-1,16)) / BvConst(DIM,16);

                size_t i = 0;
                while(i++){
                    // Keeps looping i-th row transfer till it equals row_num
                    auto continue_cond = Ite(Ugt(row_num, BvConst(i,16)), SYMBOLIC_TRUE, SYMBOLIC_FALSE); 
                    if(!checkCond(continue_cond)){
                            break;
                    }
                    size_t chunk = 0;
                    while(chunk++){
                        // Process chunks of columns at a time
                        auto continue_cond2 = Ite(Ugt(chunks, BvConst(chunk,16)), SYMBOLIC_TRUE, SYMBOLIC_FALSE);
                        if(!checkCond(continue_cond2)){
                            break;
                        }

                        auto col_start = BvConst(chunk,16) * BvConst(DIM,16);
                        int left = chunk*DIM;
                        int right;

                        auto temp = Ite(Ule(col_start+BvConst(DIM,16), col_num), SYMBOLIC_TRUE, SYMBOLIC_FALSE);
                        if(checkCond(temp)){
                            right = left+DIM-1;
                        } else {
                            right = left;
                            while(checkCond(Ite(Ult(BvConst(right+1,16), col_num), SYMBOLIC_TRUE, SYMBOLIC_FALSE))){
                                right++;
                            }
                        }   

                        auto dram_chunk_addr = DRAM_addr + (BvConst(i-1,64) * memory_stride_mvin);
                        auto dest_chunk_addr = scratchpad_addr + BvConst(i-1,32) + (ZExt(private_stride, 32) * BvConst(chunk,32)); 
                        instr.SetUpdate(scratchpad, Ite(destination == BvConst(0, 1), 
                                        scratchpad.Store(dest_chunk_addr, Extract(DRAM.Load(dram_chunk_addr), right*_Cfg.input_bits()-1, left*_Cfg.input_bits())), 
                                        scratchpad));
                        instr.SetUpdate(accumulator, Ite(destination == BvConst(1, 1),
                                        accumulator.Store(dest_chunk_addr, Extract(DRAM.Load(dram_chunk_addr), right*_Cfg.acc_bits()-1, left*_Cfg.acc_bits())), 
                                        accumulator));
                    }
                }
                
            }

            {
                // TODO
                // mvout
                InstrRef instr = m.NewInstr("mvout");
                auto decode = mvout;
                instr.SetDecode(decode);
                auto DRAM_addr = Extract(rs1, 63,0);
                auto scratchpad_addr = Extract(rs2, 31, 0);
                auto col_num = Extract(rs2, 47, 32);
                auto row_num = Extract(rs2, 64, 48);
                auto source = Extract(rs2, 31, 31);
                size_t i = 0;
                while(i++){
                    // Keeps looping i-th row transfer till it equals row_num
                    auto continue_cond = Ite(Ugt(row_num, BvConst(i,16)), SYMBOLIC_TRUE, SYMBOLIC_FALSE); 
                    if(!checkCond(continue_cond)){
                            break;
                    }
                    
                    auto dram_addr = DRAM_addr + (BvConst(i-1,64) * memory_stride_mvin);
                    auto sour_chunk_addr = scratchpad_addr + BvConst(i-1,32);
                    instr.SetUpdate(DRAM, Ite(source == BvConst(0, 1), 
                                    DRAM.Store(dram_addr, scratchpad.Load(sour_chunk_addr)), 
                                    DRAM));
                    instr.SetUpdate(DRAM, Ite(source == BvConst(1, 1),
                                    DRAM.Store(dram_addr, accumulator.Load(sour_chunk_addr)), 
                                    DRAM));
                
                }
            }
        }

        {
            // Configuration
            {
                // config_ex
                InstrRef instr = m.NewInstr("config_ex");
                auto decode = config;
                auto type = Extract(rs1,1,0);
                instr.SetDecode(decode & (rs1 == BvConst(0,2)));
                instr.SetUpdate(dataflow,Extract(rs1, 2, 2));
                instr.SetUpdate(activation_func,Extract(rs1, 3, 3));
                instr.SetUpdate(A_T,Extract(rs1, 8, 8));
                instr.SetUpdate(B_T,Extract(rs1, 9, 9));
                instr.SetUpdate(A_stride,Extract(rs1, 31, 16)); 
                instr.SetUpdate(scalar,Extract(rs1, 63, 32)); // overlap with scale?
                instr.SetUpdate(right_shift,Extract(rs2, 31, 0)); 
            }

            {
                // config_mvin
                InstrRef instr = m.NewInstr("config_mvin");
                auto decode = config;
                instr.SetDecode(decode);
                auto type = Extract(rs1,1,0);
                instr.SetDecode(decode & (rs1 == BvConst(1,2)));
                instr.SetUpdate(acc_type,Extract(rs1, 2, 2));
                instr.SetUpdate(mvin_type,Extract(rs1, 4, 3));
                instr.SetUpdate(private_stride,Extract(rs1, 31, 16)); 
                instr.SetUpdate(memory_stride_mvin,Extract(rs2, 63, 0));
                instr.SetUpdate(scale,Extract(rs1, 63, 32)); 
            }

            {
                // config_mvout
                InstrRef instr = m.NewInstr("config_mvout");
                auto decode = config;
                instr.SetDecode(decode);
                auto type = Extract(rs1,1,0);
                instr.SetDecode(decode & (rs1 == BvConst(2,2)));
                instr.SetUpdate(max_pool_stride,Extract(rs1, 5, 4));
                instr.SetUpdate(max_pool_window_size,Extract(rs1, 7, 6));
                instr.SetUpdate(upper_zero_pad,Extract(rs1, 9, 8));
                instr.SetUpdate(left_zero_pad,Extract(rs1, 11, 10));
                instr.SetUpdate(out_dim,Extract(rs1, 31, 24));
                instr.SetUpdate(pool_row,Extract(rs1, 39, 32));
                instr.SetUpdate(pool_col,Extract(rs1, 47, 40));
                instr.SetUpdate(unpool_row,Extract(rs1, 55, 48));
                instr.SetUpdate(unpool_col,Extract(rs1, 63, 56));
                instr.SetUpdate(memory_stride_mvout,Extract(rs2, 63, 0));
            }

            // TODO maybe add config_norm and flush
            // Need to find funct and behavior
        }

        // {
        //     // Core matmul sequence 
        //     {
        //         // matmul.preload
        //         InstrRef instr = m.NewInstr("matmul.preload");
        //         auto decode = matmul_preload;
        //         instr.SetDecode(decode);
        //         auto DB_scratchpad_addr = Extract(rs1, 31, 0);
        //         auto DB_col = Extract(rs1, 47, 32);
        //         auto DB_row = Extract(rs1, 63, 48);
        //         dest_addr = Extract(rs2, 31, 0);
        //         dest_col = Extract(rs2, 47, 32);
        //         dest_row = Extract(rs2, 63, 48);
        //         if (dataflow == BoolConst(false)) { // OS
        //             auto row_count = 0;
        //             auto col_count = 0;
        //             for (size_t i = BvConst(0,16); Ule(i, DB_row); i += BvConst(1,16)) {
        //                 auto& sys_row =  sys_array[row_count];
        //                 for (size_t j = BvConst(0,16); Ule(j, DB_col); j += BvConst(1,16)) {
        //                     auto index = BvConst((row_count-1)*DIM + col_count,32);
        //                     instr.SetUpdate(sys_row[col_count].D_preload_reg, scratchpad.Load(DB_scratchpad_addr+index));
        //                     col_count++;
        //                 }
        //                 row_count++;
        //             }
        //         } else { // WS
        //             auto row_count = 0;
        //             auto col_count = 0;
        //             for (size_t i = BvConst(0,16); Ule(i, DB_row); i += BvConst(1,16)) {
        //                 auto& sys_row =  sys_array[row_count];
        //                 for (size_t j = BvConst(0,16); Ule(j, DB_col); j += BvConst(1,16)) {
        //                     auto index = BvConst((row_count-1)*DIM + col_count,32);
        //                     instr.SetUpdate(sys_row[col_count].B_reg, scratchpad.Load(DB_scratchpad_addr+index));
        //                     col_count++;
        //                 }
        //                 row_count++;
        //             }
        //         }
        //     }

        //     {
        //         // matmul.compute.preloaded
        //         InstrRef instr = m.NewInstr("matmul.compute.preloaded");
        //         auto decode = matmul_compute_preloaded;
        //         instr.SetDecode(decode);
        //         auto A_scratchpad_addr = Extract(rs1, 31, 0);
        //         auto A_col = Extract(rs1, 47, 32);
        //         auto A_row = Extract(rs1, 63, 48);
        //         auto BD_scratchpad_addr = Extract(rs2, 31, 0);
        //         auto BD_col = Extract(rs2, 47, 32);
        //         auto BD_row = Extract(rs2, 63, 48);
        //         // OS C stays in the PE
        //         // WS C flows down
        //         if (dataflow == BoolConst(false)) { // OS
        //             for(size_t cycle = 0; cycle < DIM; cycle++){
        //                 // i is row, j is col
        //                 for (size_t i = 0; i < DIM; i++) {
        //                     for (size_t j = 0; i < DIM; j++) {
        //                         ExprRef A_in;
        //                         ExprRef B_in;
        //                         if(col == 0){
        //                             A_in = // TODO get address of A
        //                         } else {
        //                             A_in = sys_array[i][col-1].A_reg;
        //                         }

        //                         if(row == 0){
        //                             B_in = // TODO get address of B
        //                         } else {
        //                             B_in = sys_array[row-1][col].B_reg;
        //                         }

        //                         sys_array[row][col].A_reg = A_in;
        //                         sys_array[row][col].B_reg = B_in;

        //                         ExprRef D_val = sys_array[row][col].D_preload_reg;
        //                         auto product = sys_array[row][col].A_reg * sys_array[row][col].B_reg;
        //                         sys_array[row][col].C_reg = product + D_val;
        //                     }
        //                 }
        //             }  
        //         } else { // WS
        //             for(size_t cycle = 0; cycle < DIM; cycle++){
        //                 // i is row, j is col
        //                 for (size_t i = 0; i < DIM; i++) {
        //                     for (size_t j = 0; i < DIM; j++) {
        //                         ExprRef A_in;
        //                         ExprRef B_in;
        //                         if(col == 0){
        //                             A_in = // TODO get address of A
        //                         } else {
        //                             A_in = sys_array[i][col-1].A_reg;
        //                         }

        //                         if(row == 0){
        //                             B_in = // TODO get address of B
        //                         } else {
        //                             B_in = sys_array[row-1][col].B_reg;
        //                         }

        //                         sys_array[row][col].A_reg = A_in;
        //                         sys_array[row][col].B_reg = B_in;

        //                         ExprRef D_val = sys_array[row][col].D_preload_reg;
        //                         auto product = sys_array[row][col].A_reg * sys_array[row][col].B_reg;
        //                         sys_array[row][col].C_reg = product + D_val;

        //                         if(col < DIM - 1)
        //                             sys_array[row][col+1].A_reg = sys_array[row][col].A_reg;
        //                         if(row < DIM - 1)
        //                             sys_array[row+1][col].B_reg = sys_array[row][col].B_reg;
        //                     }
        //                 }
        //             }
        //         }
        //     }

        //     {
        //         // matmul.compute.accumulated
        //         InstrRef instr = m.NewInstr("matmul.compute.accumulated");
        //         auto decode = matmul_compute_accumulated;
        //         instr.SetDecode(decode);
        //         // TODO
        //         auto A_scratchpad_addr = Extract(rs1, 31, 0);
        //         auto A_col = Extract(rs1, 47, 32);
        //         auto A_row = Extract(rs1, 63, 48);
        //         auto BD_scratchpad_addr = Extract(rs2, 31, 0);
        //         auto BD_col = Extract(rs2, 47, 32);
        //         auto BD_row = Extract(rs2, 63, 48);

        //         if (dataflow == BoolConst(false)) { // OS
        //             for(size_t cycle = 0; cycle < DIM; cycle++){
        //                 // i is row, j is col
        //                 for (size_t i = 0; i < DIM; i++) {
        //                     for (size_t j = 0; i < DIM; j++) {
        //                         ExprRef A_in;
        //                         ExprRef B_in;
        //                         if(col == 0){
        //                             A_in = // TODO get address of A
        //                         } else {
        //                             A_in = sys_array[i][col-1].A_reg;
        //                         }

        //                         if(row == 0){
        //                             B_in = // TODO get address of B
        //                         } else {
        //                             B_in = sys_array[row-1][col].B_reg;
        //                         }

        //                         sys_array[row][col].A_reg = A_in;
        //                         sys_array[row][col].B_reg = B_in;

        //                         ExprRef D_val = sys_array[row][col].C_reg;
        //                         auto product = sys_array[row][col].A_reg * sys_array[row][col].B_reg;
        //                         sys_array[row][col].C_reg = product + D_val;
        //                     }
        //                 }
        //             }  
        //         } else { // WS
        //             for(size_t cycle = 0; cycle < DIM; cycle++){
        //                 // i is row, j is col
        //                 for (size_t i = 0; i < DIM; i++) {
        //                     for (size_t j = 0; i < DIM; j++) {
        //                         ExprRef A_in;
        //                         ExprRef B_in;
        //                         if(col == 0){
        //                             A_in = // TODO get address of A
        //                         } else {
        //                             A_in = sys_array[i][col-1].A_reg;
        //                         }

        //                         if(row == 0){
        //                             B_in = // TODO get address of B
        //                         } else {
        //                             B_in = sys_array[row-1][col].B_reg;
        //                         }

        //                         sys_array[row][col].A_reg = A_in;
        //                         sys_array[row][col].B_reg = B_in;

        //                         ExprRef D_val = sys_array[row][col].C_reg;
        //                         auto product = sys_array[row][col].A_reg * sys_array[row][col].B_reg;
        //                         sys_array[row][col].C_reg = product + D_val;

        //                         if(col < DIM - 1)
        //                             sys_array[row][col+1].A_reg = sys_array[row][col].A_reg;
        //                         if(row < DIM - 1)
        //                             sys_array[row+1][col].B_reg = sys_array[row][col].B_reg;
        //                     }
        //                 }
        //             }
        //         }
        //     }
        // }

        // Note: Loop instructions not included in README, therefore not implemented for now
    }

    

}