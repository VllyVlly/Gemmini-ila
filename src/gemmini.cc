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
        // ---------- Extra for mvin and mvout----
        mvin_DRAM_addr(m.NewBvState("mvin_DRAM_addr",64)),
        mvin_dest_addr(m.NewBvState("mvin_dest_addr",32)),
        mvin_col_num(m.NewBvState("mvin_col_num",16)),
        mvin_row_num(m.NewBvState("mvin_row_num",16)),
        mvin_destination(m.NewBvState("mvin_destination",1)),
        mvout_DRAM_addr(m.NewBvState("mvout_DRAM_addr",64)),
        mvout_sour_addr(m.NewBvState("mvout_sour_addr",32)),
        mvout_col_num(m.NewBvState("mvout_col_num",16)),
        mvout_row_num(m.NewBvState("mvout_row_num",16)),
        mvout_source(m.NewBvState("mvout_source",1)),
        start_row(m.NewBvState("start_row",16)),
        start_chunk(m.NewBvState("start_chunk",16)),
        done(m.NewBoolState("done")),
        // ---------- Extra for preload ----------
        dest_addr(m.NewBvState("dest_addr", 32)),
        dest_row(m.NewBvState("dest_row", 32)),
        dest_col(m.NewBvState("dest_col", 32)), 
        DRAM(m.NewMemState("DRAM", 64, 32)),
        scratchpad(m.NewMemState("scratchpad", 32, Cfg.DIM * Cfg.input_bits)),
        accumulator(m.NewMemState("accumulator", 32, Cfg.DIM * Cfg.acc_bits))
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

        // Make this more flexible
        constexpr size_t DIM = 16;
        constexpr size_t INPUT_BITS = _Cfg.input_bits;
        constexpr size_t ACC_BITS = _Cfg.acc_bits;
        constexpr size_t INPUT_ROW_BITS = DIM * INPUT_BITS;
        constexpr size_t ACC_ROW_BITS = DIM * ACC_BITS;
        
        {
            
            // Data Movement
            // rs1 is DRAM address
            {
                // mvin

                // mvin start
                {
                    InstrRef instr = m.NewInstr("mvin");
                    auto decode = mvin;
                    instr.SetDecode(funct == decode);
                    ExprRef DRAM_addr = Extract(rs1, 63,0);
                    ExprRef dest_addr = Extract(rs2, 31, 0);
                    ExprRef col_num = Extract(rs2, 47, 32);
                    ExprRef row_num = Extract(rs2, 63, 48);
                    ExprRef destination = Extract(rs2, 31, 31);

                    instr.SetUpdate(start_row, BvConst(0, 16));
                    instr.SetUpdate(start_chunk, BvConst(0, 16));
                    instr.SetUpdate(done, BoolConst(false));
                    instr.SetUpdate(mvin_DRAM_addr, DRAM_addr);
                    instr.SetUpdate(mvin_dest_addr, dest_addr);
                    instr.SetUpdate(mvin_row_num, row_num);
                    instr.SetUpdate(mvin_col_num, col_num);
                    instr.SetUpdate(mvin_destination, destination);
                }

                // mvin step
                {
                    InstrRef instr = m.NewInstr("mvin_step");
                    instr.SetDecode((funct == mvin) & !done);

                    // Total chunks per row
                    auto total_chunks = (mvin_col_num + BvConst(DIM - 1, 16)) / BvConst(DIM, 16);
                    
                    // Determine how many columns to process per chunk
                    auto col_start = start_chunk * BvConst(DIM, 16);
                    auto chunk_end = Ite((col_start + BvConst(DIM, 16)) <= mvin_col_num,
                                        col_start + BvConst(DIM, 16),
                                        mvin_col_num);
                    auto chunk_width = chunk_end - col_start;
                    
                    auto chunk_valid = Ite(Ult(col_start,mvin_col_num), SYMB_TRUE, SYMB_FALSE);
                    auto should_transfer = Ite(!done & chunk_valid, SYMB_TRUE, SYMB_FALSE);
                    
                    auto dram_base = mvin_DRAM_addr + (ZExt(start_row,64) * memory_stride_mvin) + (ZExt(start_chunk, 64) * BvConst(DIM,64));
                    auto dest_base = mvin_dest_addr + (ZExt(start_row, 32)) + (ZExt(start_chunk, 32) * ZExt(private_stride, 32));
                    
                    auto current_row_input = scratchpad.Load(dest_base);
                    auto current_row_acc = accumulator.Load(dest_base);
                    ExprRef new_row_input_concat = BvConst(0, INPUT_ROW_BITS);
                    ExprRef new_row_acc_concat = BvConst(0, ACC_ROW_BITS);
                    
                    for (size_t elem = 0; elem < DIM; elem++) {
                        auto elem_valid = Ite(BvConst(elem, 16) < chunk_width, SYMB_TRUE, SYMB_FALSE);
                        auto should_update = Ite(should_transfer & elem_valid, SYMB_TRUE, SYMB_FALSE);

                        auto dram_elem_addr = dram_base + BvConst(elem, 64); // multiply by bitwidth?
                        auto load_elem = DRAM.Load(dram_elem_addr);
                        
                        auto load_elem_input = Extract(load_elem, INPUT_BITS - 1, 0);
                        auto load_elem_acc = Extract(load_elem, ACC_BITS - 1, 0);
                        
                        if (elem == 0) {
                            new_row_input_concat = load_elem_input;
                            new_row_acc_concat = load_elem_acc;
                        } else {
                            auto new_elem_input = Concat(load_elem_input, new_row_input_concat);
                            auto new_elem_acc = Concat(load_elem_acc, new_row_acc_concat);
                            
                            auto old_padded_input = ZExt(new_row_input_concat, INPUT_BITS + (elem * INPUT_BITS));
                            auto old_padded_acc = ZExt(new_row_acc_concat, ACC_BITS + (elem * ACC_BITS));
                            
                            new_row_input_concat = Ite(should_update, new_elem_input, old_padded_input);
                            new_row_acc_concat = Ite(should_update, new_elem_acc, old_padded_acc);
                        }
                    }

                    // Decide whether to update or not
                    auto store_sp = scratchpad.Store(dest_base, new_row_input_concat);
                    auto store_acc_input = accumulator.Store(dest_base, ZExt(new_row_input_concat, ACC_ROW_BITS));
                    auto store_acc = accumulator.Store(dest_base, new_row_acc_concat);
                    instr.SetUpdate(scratchpad, Ite(should_transfer & (mvin_destination == BvConst(0, 1)), store_sp, scratchpad));
                    instr.SetUpdate(accumulator, Ite(should_transfer & (mvin_destination == BvConst(1, 1)), Ite(acc_type == BvConst(0,1), store_acc, store_acc_input), accumulator));
                    
                    // Advance chunk counter
                    auto next_chunk = start_chunk + BvConst(1, 16);
                    auto chunk_overflow = Ite(Uge(next_chunk,total_chunks), SYMB_TRUE, SYMB_FALSE);
                    auto new_chunk = Ite(chunk_overflow, BvConst(0, 16), next_chunk);
                    auto new_row = Ite(chunk_overflow, start_row + BvConst(1, 16), start_row);
                    
                    instr.SetUpdate(start_chunk, Ite(!done, new_chunk, start_chunk));
                    instr.SetUpdate(start_row, Ite(!done, new_row, start_row));
                    
                    // Set done flag when all done
                    auto all_rows_done = Uge(new_row, mvin_row_num);
                    auto done_after = Ite(chunk_overflow & all_rows_done, SYMB_TRUE, SYMB_FALSE);
                    instr.SetUpdate(done, Ite(done_after, BoolConst(true), done));
                }

                // mvin end
                // TODO make sure that config and done resets
                {
                    InstrRef instr = m.NewInstr("mvin_end");
                    instr.SetDecode(funct == mvin & done);
                }
            }

            {
                // mvout start
                {
                    InstrRef instr = m.NewInstr("mvout");
                    auto decode = mvout;
                    instr.SetDecode(funct == decode);
                    auto DRAM_addr = Extract(rs1, 63,0);
                    auto sour_addr = Extract(rs2, 31, 0);
                    auto col_num = Extract(rs2, 47, 32);
                    auto row_num = Extract(rs2, 63, 48);
                    auto source = Extract(rs2, 31, 31);

                    instr.SetUpdate(start_row, BvConst(0, 16));
                    instr.SetUpdate(start_chunk, BvConst(0, 16));
                    instr.SetUpdate(done, BoolConst(false));
                    instr.SetUpdate(mvout_DRAM_addr, DRAM_addr);
                    instr.SetUpdate(mvout_sour_addr, sour_addr);
                    instr.SetUpdate(mvout_row_num, row_num);
                    instr.SetUpdate(mvout_col_num, col_num);
                    instr.SetUpdate(mvout_source, source);
                }

                // mvout step
                {
                    InstrRef instr = m.NewInstr("mvout_step");
                    auto decode = mvout;
                    instr.SetDecode((funct == decode) & !done);


                }

                // mvout end
                {
                    InstrRef instr = m.NewInstr("mvout_end");
                    auto decode = mvout;
                    instr.SetDecode((funct == decode) & done);
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
                instr.SetDecode((funct == decode) & (type == BvConst(0,2)));
                instr.SetUpdate(dataflow,Ite(Extract(rs1, 2, 2) == BvConst(1,1), BoolConst(true), BoolConst(false)));
                instr.SetUpdate(activation_func,Extract(rs1, 3, 3));
                instr.SetUpdate(A_T,Extract(rs1, 8, 8));
                instr.SetUpdate(B_T,Extract(rs1, 9, 9));
                instr.SetUpdate(A_stride,Extract(rs1, 31, 16)); 
                instr.SetUpdate(scalar,Extract(rs1, 63, 32));
                instr.SetUpdate(right_shift,Extract(rs2, 31, 0)); 
            }

            {
                // config_mvin
                InstrRef instr = m.NewInstr("config_mvin");
                auto decode = config;
                auto type = Extract(rs1,1,0);
                instr.SetDecode((funct == decode) & (type == BvConst(1,2)));
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
                auto type = Extract(rs1,1,0);
                instr.SetDecode((funct == decode) & (type == BvConst(2,2)));
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

        {
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

}