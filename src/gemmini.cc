#include "gemmini.h"

#include <bits/stdc++.h>
#include <ilang/ilang++.h>

namespace gemmini{

    Gemmini::Gemmini():
        m(Ila("Gemmini")),
        // Parameters
        tileRows(m.NewBvState("tileRows", TEMP_BIT_WIDTH)),
        tileColumns(m.NewBvState("tileColumns", TEMP_BIT_WIDTH)),
        meshRows(m.NewBvState("meshRows", TEMP_BIT_WIDTH)),
        meshColumns(m.NewBvState("meshColumns", TEMP_BIT_WIDTH)),
        dataflow(m.NewBoolState("dataflow")),
        sp_banks(m.NewBvState("sp_banks", TEMP_BIT_WIDTH)),
        sp_capacity(m.NewBvState("sp_capacity", TEMP_BIT_WIDTH)),
        acc_capacity(m.NewBvState("acc_capacity", TEMP_BIT_WIDTH)),
        inputType(m.NewBvState("inputType", TEMP_BIT_WIDTH)),
        outputType(m.NewBvState("outputType", TEMP_BIT_WIDTH)),
        accType(m.NewBvState("accType", TEMP_BIT_WIDTH)),
        ld_queue_length(m.NewBvState("ld_queue_length", TEMP_BIT_WIDTH)),
        st_queue_length(m.NewBvState("st_queue_length", TEMP_BIT_WIDTH)),
        ex_queue_length(m.NewBvState("ex_queue_length", TEMP_BIT_WIDTH)),
        rob_entries(m.NewBvState("rob_entries", TEMP_BIT_WIDTH)),
        dma_maxbytes(m.NewBvState("dma_maxbytes", TEMP_BIT_WIDTH)),
        dma_buswidth(m.NewBvState("dma_buswidth", TEMP_BIT_WIDTH)),
        mem_pipeline(m.NewBvState("mem_pipeline", TEMP_BIT_WIDTH)),
        // Input
        funct(m.NewBvInput("funct", 3)),
        rs1(m.NewBvInput("rs1", RS_WIDTH)),
        rs2(m.NewBvInput("rs2", RS_WIDTH)),
        // Config
        shift(m.NewBvState("shift", 32)),
        A_stride(m.NewBvState("A_stride", 16)),
        scale(m.NewBvState("scale", 32)),
        private_stride(m.NewBvState("private_stride", 16)),
        memory_stride(m.NewBvState("stride", 64)),
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
        // Extra for preload
        dest_addr(m.NewBvState("dest_addr",32)),
        dest_row(m.NewBvState("dest_row",32)),
        dest_col(m.NewBvState("dest_col",32))
        // Memory
        {
            // TODO change DEFAULT_DIM
            // TODO actl figure out how to get the address width
            DIM = DEFAULT_DIM;
            DRAM = m.NewMemState("DRAM", 64, TEMP_BIT_WIDTH);

            // Scratchpad and accumulator store rows of data per address
            scratchpad = m.NewMemState("s", 32, DIM*INPUT_TYPE_BIT_WIDTH);                 
            accumulator = m.NewMemState("a", 32, DIM*ACC_TYPE_BIT_WIDTH);
            
            sys_array.resize(DIM);

            // TODO Make tile seperations
            for (size_t i = 0; i < DIM; i++) {
                auto& row = sys_array[i];
                for(size_t j = 0; j < DIM; j++){
                    row.push_back(PE(m, i, j));
                }
            }

        }

    Gemmini::AddInstructions(){

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
                int row_count = 0;
                
                auto destination = Extract(rs2, 31, 31);
                auto chunks = (col_num + BvConst(DIM-1,16)) / BvConst(DIM,16);
                if(destination == BvConst(0,1)) { // to scratchpad
                    size_t i = 0;
                    while(i++){
                        auto continue_cond = Ite(Ugt(row_num, BvConst(i,16)), BoolConst(true), BoolConst(false)); // Continue while i < row_num

                        int chunk_count = 0;

                        size_t chunk = 0;
                        while(chunk++){
                            auto continue_cond2 = Ite(Ugt(chunks, BvConst(chunk,16)), BoolConst(true), BoolConst(false)); // Continue while chunk < chunks

                            auto col_start = BvConst(chunk,16) * BvConst(DIM,16);
                            int left = chunk_count*DIM;
                            int right;

                            auto temp = Ite(Ule(col_start+BvConst(DIM,16), col_num), BoolConst(true), BoolConst(false));
                            if(temp == BoolConst(true)){
                                right = left+DIM-1;
                            } else {
                                right = left;
                                while(Ite(Ult(BvConst(right+1,16), col_num), BoolConst(true), BoolConst(false)) == BoolConst(true)){
                                    right++;
                                }
                            }   

                            auto dram_chunk_addr = DRAM_addr + (BvConst(row_count-1,64) * memory_stride);
                            auto sp_chunk_addr = scratchpad_addr + BvConst(row_count-1,32) + (ZExt(private_stride, 32) * BvConst(chunk,32));
                            instr.SetUpdate(scratchpad, scratchpad.Store(sp_chunk_addr, Extract(DRAM.Load(dram_chunk_addr), right*INPUT_TYPE_BIT_WIDTH-1, left*INPUT_TYPE_BIT_WIDTH)));
                            
                            chunk_count++;

                            if(continue_cond2 == BoolConst(false))
                                break;
                        }

                        row_count++;
                        if(continue_cond == BoolConst(false))
                            break;
                    }
                    
                } else { // to accumulator
                    size_t i = 0;
                    while(i++){
                        auto continue_cond = Ite(Ugt(row_num, BvConst(i,16)), BoolConst(true), BoolConst(false)); // Continue while i < row_num

                        int chunk_count = 0;

                        size_t chunk = 0;
                        while(chunk++){
                            auto continue_cond2 = Ite(Ugt(chunks, BvConst(chunk,16)), BoolConst(true), BoolConst(false)); // Continue while chunk < chunks

                            auto col_start = BvConst(chunk,16) * BvConst(DIM,16);
                            int left = chunk_count*DIM;
                            int right;

                            auto temp = Ite(Ule(col_start+BvConst(DIM,16), col_num), BoolConst(true), BoolConst(false));
                            if(temp == BoolConst(true)){
                                right = left+DIM-1;
                            } else {
                                right = left;
                                while(Ite(Ult(BvConst(right+1,16), col_num), BoolConst(true), BoolConst(false)) == BoolConst(true)){
                                    right++;
                                }
                            }   

                            auto dram_chunk_addr = DRAM_addr + (BvConst(row_count-1,64) * memory_stride);
                            auto acc_chunk_addr = scratchpad_addr + BvConst(row_count-1,32) + (ZExt(private_stride, 32) * BvConst(chunk,32));
                            instr.SetUpdate(accumulator, accumulator.Store(acc_chunk_addr, Extract(DRAM.Load(dram_chunk_addr), right*ACC_TYPE_BIT_WIDTH-1, left*ACC_TYPE_BIT_WIDTH)));
                            
                            chunk_count++;

                            if(continue_cond2 == BoolConst(false))
                                break;
                        }

                        row_count++;
                        if(continue_cond == BoolConst(false))
                            break;
                    }
                }
                // Figure out how to get the index for moving the columns better
                // Actl test it
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
                auto row_count = 0;
                auto col_count = 0;
                for (size_t i = BvConst(0,16); Ule(i, row_num); i += BvConst(1,16)) {
                    for (size_t j = BvConst(0,16); Ule(j, col_num); j += BvConst(1,16)) {
                        auto s_index = BvConst((row_count-1)*DIM + col_count,32);
                        auto m_index = stride*BvConst((row_count-1)*DIM + col_count,64)
                        instr.SetUpdate(DRAM, DRAM.Store(DRAM_addr+m_index, scratchpad.Load(scratchpad_addr+s_index)));
                        col_count++;
                    }
                    row_count++;
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
                instr.SetUpdate(memory_stride,Extract(rs2, 63, 0));
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
            }

            // TODO maybe add config_norm and flush
            // Need to find funct and behavior
        }

        {
            // Core matmul sequence 
            {
                // matmul.preload
                InstrRef instr = m.NewInstr("matmul.preload");
                auto decode = matmul_preload;
                instr.SetDecode(decode);
                auto DB_scratchpad_addr = Extract(rs1, 31, 0);
                auto DB_col = Extract(rs1, 47, 32);
                auto DB_row = Extract(rs1, 63, 48);
                dest_addr = Extract(rs2, 31, 0);
                dest_col = Extract(rs2, 47, 32);
                dest_row = Extract(rs2, 63, 48);
                if (dataflow == BoolConst(false)) { // OS
                    auto row_count = 0;
                    auto col_count = 0;
                    for (size_t i = BvConst(0,16); Ule(i, DB_row); i += BvConst(1,16)) {
                        auto& sys_row =  sys_array[row_count];
                        for (size_t j = BvConst(0,16); Ule(j, DB_col); j += BvConst(1,16)) {
                            auto index = BvConst((row_count-1)*DIM + col_count,32);
                            instr.SetUpdate(sys_row[col_count].D_preload_reg, scratchpad.Load(DB_scratchpad_addr+index));
                            col_count++;
                        }
                        row_count++;
                    }
                } else { // WS
                    auto row_count = 0;
                    auto col_count = 0;
                    for (size_t i = BvConst(0,16); Ule(i, DB_row); i += BvConst(1,16)) {
                        auto& sys_row =  sys_array[row_count];
                        for (size_t j = BvConst(0,16); Ule(j, DB_col); j += BvConst(1,16)) {
                            auto index = BvConst((row_count-1)*DIM + col_count,32);
                            instr.SetUpdate(sys_row[col_count].B_reg, scratchpad.Load(DB_scratchpad_addr+index));
                            col_count++;
                        }
                        row_count++;
                    }
                }
            }

            {
                // matmul.compute.preloaded
                InstrRef instr = m.NewInstr("matmul.compute.preloaded");
                auto decode = matmul_compute_preloaded;
                instr.SetDecode(decode);
                auto A_scratchpad_addr = Extract(rs1, 31, 0);
                auto A_col = Extract(rs1, 47, 32);
                auto A_row = Extract(rs1, 63, 48);
                auto BD_scratchpad_addr = Extract(rs2, 31, 0);
                auto BD_col = Extract(rs2, 47, 32);
                auto BD_row = Extract(rs2, 63, 48);
                // OS C stays in the PE
                // WS C flows down
                if (dataflow == BoolConst(false)) { // OS
                    for(size_t cycle = 0; cycle < DIM; cycle++){
                        // i is row, j is col
                        for (size_t i = 0; i < DIM; i++) {
                            for (size_t j = 0; i < DIM; j++) {
                                ExprRef A_in;
                                ExprRef B_in;
                                if(col == 0){
                                    A_in = // TODO get address of A
                                } else {
                                    A_in = sys_array[i][col-1].A_reg;
                                }

                                if(row == 0){
                                    B_in = // TODO get address of B
                                } else {
                                    B_in = sys_array[row-1][col].B_reg;
                                }

                                sys_array[row][col].A_reg = A_in;
                                sys_array[row][col].B_reg = B_in;

                                ExprRef D_val = sys_array[row][col].D_preload_reg;
                                auto product = sys_array[row][col].A_reg * sys_array[row][col].B_reg;
                                sys_array[row][col].C_reg = product + D_val;
                            }
                        }
                    }  
                } else { // WS
                    for(size_t cycle = 0; cycle < DIM; cycle++){
                        // i is row, j is col
                        for (size_t i = 0; i < DIM; i++) {
                            for (size_t j = 0; i < DIM; j++) {
                                ExprRef A_in;
                                ExprRef B_in;
                                if(col == 0){
                                    A_in = // TODO get address of A
                                } else {
                                    A_in = sys_array[i][col-1].A_reg;
                                }

                                if(row == 0){
                                    B_in = // TODO get address of B
                                } else {
                                    B_in = sys_array[row-1][col].B_reg;
                                }

                                sys_array[row][col].A_reg = A_in;
                                sys_array[row][col].B_reg = B_in;

                                ExprRef D_val = sys_array[row][col].D_preload_reg;
                                auto product = sys_array[row][col].A_reg * sys_array[row][col].B_reg;
                                sys_array[row][col].C_reg = product + D_val;

                                if(col < DIM - 1)
                                    sys_array[row][col+1].A_reg = sys_array[row][col].A_reg;
                                if(row < DIM - 1)
                                    sys_array[row+1][col].B_reg = sys_array[row][col].B_reg;
                            }
                        }
                    }
                }
            }

            {
                // matmul.compute.accumulated
                InstrRef instr = m.NewInstr("matmul.compute.accumulated");
                auto decode = matmul_compute_accumulated;
                instr.SetDecode(decode);
                // TODO
                auto A_scratchpad_addr = Extract(rs1, 31, 0);
                auto A_col = Extract(rs1, 47, 32);
                auto A_row = Extract(rs1, 63, 48);
                auto BD_scratchpad_addr = Extract(rs2, 31, 0);
                auto BD_col = Extract(rs2, 47, 32);
                auto BD_row = Extract(rs2, 63, 48);

                if (dataflow == BoolConst(false)) { // OS
                    for(size_t cycle = 0; cycle < DIM; cycle++){
                        // i is row, j is col
                        for (size_t i = 0; i < DIM; i++) {
                            for (size_t j = 0; i < DIM; j++) {
                                ExprRef A_in;
                                ExprRef B_in;
                                if(col == 0){
                                    A_in = // TODO get address of A
                                } else {
                                    A_in = sys_array[i][col-1].A_reg;
                                }

                                if(row == 0){
                                    B_in = // TODO get address of B
                                } else {
                                    B_in = sys_array[row-1][col].B_reg;
                                }

                                sys_array[row][col].A_reg = A_in;
                                sys_array[row][col].B_reg = B_in;

                                ExprRef D_val = sys_array[row][col].C_reg;
                                auto product = sys_array[row][col].A_reg * sys_array[row][col].B_reg;
                                sys_array[row][col].C_reg = product + D_val;
                            }
                        }
                    }  
                } else { // WS
                    for(size_t cycle = 0; cycle < DIM; cycle++){
                        // i is row, j is col
                        for (size_t i = 0; i < DIM; i++) {
                            for (size_t j = 0; i < DIM; j++) {
                                ExprRef A_in;
                                ExprRef B_in;
                                if(col == 0){
                                    A_in = // TODO get address of A
                                } else {
                                    A_in = sys_array[i][col-1].A_reg;
                                }

                                if(row == 0){
                                    B_in = // TODO get address of B
                                } else {
                                    B_in = sys_array[row-1][col].B_reg;
                                }

                                sys_array[row][col].A_reg = A_in;
                                sys_array[row][col].B_reg = B_in;

                                ExprRef D_val = sys_array[row][col].C_reg;
                                auto product = sys_array[row][col].A_reg * sys_array[row][col].B_reg;
                                sys_array[row][col].C_reg = product + D_val;

                                if(col < DIM - 1)
                                    sys_array[row][col+1].A_reg = sys_array[row][col].A_reg;
                                if(row < DIM - 1)
                                    sys_array[row+1][col].B_reg = sys_array[row][col].B_reg;
                            }
                        }
                    }
                }
            }
        }

        // Note: Loop instructions not included in README, therefore not implemented for now
    }

    

}