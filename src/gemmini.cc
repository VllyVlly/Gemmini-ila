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
        // ---------- Extra for mvin -------------
        mvin_DRAM_addr(m.NewBvState("mvin_DRAM_addr",64)),
        mvin_dest_addr(m.NewBvState("mvin_dest_addr",32)),
        mvin_col_num(m.NewBvState("mvin_col_num",16)),
        mvin_row_num(m.NewBvState("mvin_row_num",16)),
        mvin_destination(m.NewBvState("mvin_destination",1)),
        start_row(m.NewBvState("start_row",16)),
        start_chunk(m.NewBvState("start_chunk",16)),
        done(m.NewBoolState("done")),
        // ---------- Extra for preload ----------
        dest_addr(m.NewBvState("dest_addr", 32)),
        dest_row(m.NewBvState("dest_row", 32)),
        dest_col(m.NewBvState("dest_col", 32)), 
        DRAM(m.NewMemState("DRAM", 64, 32)),
        scratchpad(m.NewMemState("scratchpad", 32, Cfg.DIM * Cfg.input_bits())),
        accumulator(m.NewMemState("accumulator", 32, Cfg.DIM * Cfg.acc_bits()))
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

                // mvin start
                {
                    InstrRef instr = m.NewInstr("mvin");
                    auto decode = mvin;
                    instr.SetDecode(funct == decode);
                    auto DRAM_addr = Extract(rs1, 63,0);
                    auto dest_addr = Extract(rs2, 31, 0);
                    auto col_num = Extract(rs2, 47, 32);
                    auto row_num = Extract(rs2, 64, 48);
                    auto destination = Extract(rs2, 31, 31);

                    instr.SetUpdate(start_row, BvConst(0, 16));
                    instr.SetUpdate(start_chunk, BvConst(0, 16));
                    instr.SetUpdate(dma_done, BoolConst(false));
                    instr.SetUpdate(mvin_DRAM_addr, DRAM_addr);
                    instr.SetUpdate(mvin_dest_addr, dest_addr);
                    instr.SetUpdate(mvin_row_num, row_num);
                    instr.SetUpdate(mvin_col_num, col_num);
                    instr.SetUpdate(mvin_destination, destination);
                }

                // mvin step
                {
                    InstrRef instr = m.NewInstr("mvin_step_chunk");
                    instr.SetDecode((funct == mvin) & !done);
                    
                    // Total chunks per row
                    auto total_chunks = (col_num + BvConst(DIM - 1, 16)) / BvConst(DIM, 16);
                    auto row_done = Ite(Uge(start_row,mvin_row_num), SYMB_TRUE, SYMB_FALSE);
                    auto chunk_done = Ite(Uge(start_chunk,total_chunks), SYMB_TRUE, SYMB_FALSE);
                    auto all_done = Ite((row_done & chunk_done), SYMB_TRUE, SYMB_FALSE);
                    
                    auto col_start = start_chunk * BvConst(DIM, 16);
                    auto chunk_end = Ite((col_start + BvConst(DIM, 16)) <= mvin_col_num,
                                        col_start + BvConst(DIM, 16),
                                        mvin_col_num);
                    auto chunk_width = chunk_end - col_start;
                    
                    auto chunk_valid = Ite(Ult(col_start,mvin_col_num), SYMB_TRUE, SYMB_FALSE);
                    auto should_transfer = Ite(!all_done & chunk_valid, SYMB_TRUE, SYMB_FALSE);
                    
                    // Get the row
                    auto dram_base = mvin_DRAM_addr + (start_row * memory_stride_mvin) + (start_chunk * BvConst(DIM,64));
                    auto dest_base = mvin_dest_addr + (start_row) + (start_chunk * private_stride);
                    
                    auto current_row_input = scratchpad.Load(dest_base);
                    auto current_row_acc = accumulator.Load(dest_base);

                    // Build new row from DRAM (or load existing)
                    auto new_row_input = current_row_input;  // Start with current row
                    auto new_row_acc = current_row_acc;

                    for (size_t elem = 0; elem < DIM; elem++) {
                        auto elem_valid = Ite(BvConst(elem, 16) < chunk_width, SYMB_TRUE, SYMB_FALSE);
                        auto should_update = Ite(should_transfer & elem_valid, SYMB_TRUE, SYMB_FALSE);
                        
                        // Load element from DRAM
                        auto dram_elem_addr = dram_base + BvConst(elem, 64);
                        auto load_elem = DRAM.Load(dram_elem_addr);
                        
                        // Create mask for this element
                        auto elem_mask_input = BvConst(((1ULL << _Cfg.input_bits()) - 1) << (elem * _Cfg.input_bits()), DIM * _Cfg.input_bits());
                        auto elem_mask_acc = BvConst(((1ULL << _Cfg.acc_bits()) - 1) << (elem * _Cfg.acc_bits()), DIM * _Cfg.acc_bits());
                        
                        // Clear the element's bits in the new row
                        auto row_cleared_input = new_row & ~elem_mask_input;
                        auto row_cleared_acc= new_row & ~elem_mask_acc;
                        
                        // Insert the new element
                        auto shifted_elem_input = ZExt(load_elem, DIM * _Cfg.input_bits()) << (elem * _Cfg.input_bits());
                        auto shifted_elem_acc = ZExt(load_elem, DIM * _Cfg.acc_bits()) << (elem * _Cfg.acc_bits());
                        auto row_updated_input = row_cleared | shifted_elem_input;
                        auto row_updated_acc = row_cleared | shifted_elem_acc;
                        
                        // Conditionally update
                        new_row_input = Ite(should_update, row_updated_input, new_row_input);
                        new_row_acc = Ite(should_update, row_updated_acc, new_row_acc);
                    }
                    
                    auto store_sp = scratchpad.Store(dest_base, new_row_input);
                    auto store_acc = accumulator.Store(dest_base, new_row_acc);
                    instr.SetUpdate(scratchpad, Ite(should_transfer & mvin_destination == BvConst(0, 1), final_sp_store, scratchpad));
                    instr.SetUpdate(accumulator, Ite(should_transfer & mvin_destination == BvConst(1, 1), final_acc_store, accumulator));
                    
                    // Advance chunk counter
                    auto next_chunk = start_chunk + BvConst(1, 16);
                    auto chunk_overflow = Ite(Uge(next_chunk,total_chunks), SYMB_TRUE, SYMB_FALSE);
                    auto new_chunk = Ite(chunk_overflow, BvConst(0, 16), next_chunk);
                    auto new_row = Ite(chunk_overflow, start_row + BvConst(1, 16), start_row);
                    
                    instr.SetUpdate(start_chunk, Ite(!all_done, new_chunk, start_chunk));
                    instr.SetUpdate(start_row, Ite(!all_done, new_row, start_row));
                    
                    // Set done flag when all done
                    auto all_rows_done = Ite(Uge(new_row,mvin_row_num), SYMB_TRUE, SYMB_FALSE);
                    auto done_after = Ite(chunk_overflow & all_rows_done, SYMB_TRUE, SYMB_FALSE);
                    instr.SetUpdate(done, Ite(done_after, BoolConst(true), done));
                }

                // mvin end
                // TODO make sure that config and done resets
                {
                    InstrRef instr = m.NewInstr("mvin_end");
                    instr.SetDecode(m.And(m.Eq(funct, mvin), dma_done));
                    
                    // Optional: any cleanup or signaling
                    // For now, just stay in done state
                
                }
            }

            {
                // mvout
                InstrRef instr = m.NewInstr("mvout");
                auto decode = mvout;
                instr.SetDecode(funct == decode);
                auto DRAM_addr = Extract(rs1, 63,0);
                auto scratchpad_addr = Extract(rs2, 31, 0);
                auto col_num = Extract(rs2, 47, 32);
                auto row_num = Extract(rs2, 64, 48);
                auto source = Extract(rs2, 31, 31);
                size_t i = 0;
                
                auto continue_cond = Ite(Ugt(row_num, BvConst(i,16)), SYMBOLIC_TRUE, SYMBOLIC_FALSE); 
                if(checkCond(continue_cond)){
                    auto dram_addr = DRAM_addr + (BvConst(i,64) * memory_stride_mvin);
                    auto sour_chunk_addr = scratchpad_addr + BvConst(i,32);
                    auto source_data = Ite(source == BvConst(0, 1), scratchpad.Load(sour_chunk_addr), accumulator.Load(sour_chunk_addr));
                    instr.SetUpdate(DRAM, DRAM.Store(dram_addr, source_data));
                    i++;
                }
                while(i++){
                    // Keeps looping i-th row transfer till it equals row_num
                    auto continue_cond = Ite(Ugt(row_num, BvConst(i,16)), SYMBOLIC_TRUE, SYMBOLIC_FALSE); 
                    if(!checkCond(continue_cond)){
                        break;
                    }
                    
                    auto dram_addr = DRAM_addr + (BvConst(i,64) * memory_stride_mvin);
                    auto sour_chunk_addr = scratchpad_addr + BvConst(i,32);
                    auto source_data = Ite(source == BvConst(0, 1), scratchpad.Load(sour_chunk_addr), accumulator.Load(sour_chunk_addr));
                    instr.SetUpdate(DRAM, DRAM.Store(dram_addr, source_data));
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