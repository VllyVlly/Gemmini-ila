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
        unpool_col(m.NewBvState("unpool_col", 8))
        // Memory
        {
            // DIM x DIM SYSTOLIC ARRAY
            // TODO change DEFAULT_DIM
            auto DIM = DEFAULT_DIM;
            auto sp_rows = sp_capacity / (DIM * sizeof(inputType)) // NOT FINAL

            for (size_t i = 0; i < sp_rows; i++) {
                scratchpad.push_back(m.NewMemState(
                    "s" + std::to_string(i),
                    INPUT_TYPE_BIT_WIDTH,  
                    DIM                      
                ));
            }

            for (size_t i = 0; i < sp_rows; i++) {
                accumulator.push_back(m.NewMemState(
                    "a" + std::to_string(i),
                    ACC_TYPE_BIT_WIDTH,    
                    DIM                     
                ));
            }

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
                auto scratchpad_addr = Extract(rs2, 31, 0);
                auto col_num = Extract(rs2, 47, 32);
                auto row_num = Extract(rs2, 64, 48);
            }

            {
                // mvout
                InstrRef instr = m.NewInstr("mvout");
                auto decode = mvout;
                instr.SetDecode(decode);
                auto scratchpad_addr = Extract(rs2, 31, 0);
                auto col_num = Extract(rs2, 47, 32);
                auto row_num = Extract(rs2, 64, 48);
            }
        }

        {
            // Configuration
            {
                // config_ex
                InstrRef instr = m.NewInstr("config_ex");
                auto decode = config;
                instr.SetDecode(decode);
                // rs1[1:0] must be 00, how tf do i force this
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
                // rs1[1:0] must be 01
                instr.SetUpdate(acc_type,Extract(rs1, 2, 2));
                instr.SetUpdate(mvin_type,Extract(rs1, 4, 3));
                instr.SetUpdate(stride,Extract(rs1, 31, 16)); 
                instr.SetUpdate(scale,Extract(rs1, 63, 32)); 
            }

            {
                // config_mvout
                InstrRef instr = m.NewInstr("config_mvout");
                auto decode = config;
                instr.SetDecode(decode);
                // rs1[1:0] must be 10
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
                auto C_scratchpad_addr = Extract(rs2, 31, 0);
                auto C_col = Extract(rs2, 47, 32);
                auto C_row = Extract(rs2, 63, 48);
                if(dataflow == BoolConst(false)){ // OS
                    for (size_t i = 0; i < DIM; i++) {
                        auto& sys_row =  sys_array[i];
                        auto& scratchpad_row = m.state("s"+std::to_string(i));
                        
                        for (size_t j = 0; i < DIM; j++) {
                            instr.SetUpdate(sys_row[j].D_preload_reg, Store(sys_row, BvConst(j,DIM_WIDTH), Load(scratchpad_row, BvConst(j,DIM_WIDTH))));
                        }
                    }
                } else { // WS
                    for (size_t i = 0; i < DIM; i++) {
                        auto& sys_row =  sys_array[i];
                        auto& scratchpad_row = m.state("s"+std::to_string(i));
                        
                        for (size_t j = 0; i < DIM; j++) {
                            instr.SetUpdate(sys_row[j].B_reg, Store(sys_row, BvConst(j,DIM_WIDTH), Load(scratchpad_row, BvConst(j,DIM_WIDTH))));
                        }
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
                // NOT DONE
            }

            {
                // matmul.compute.accumulated
                InstrRef instr = m.NewInstr("matmul.compute.accumulated");
                auto decode = matmul_compute_accumulated;
                instr.SetDecode(decode);
                // TODO
                // If output-stationary, this instruction will compute on the 
                // previously computed result (C) in the systolic array, accumulating on top of it
                // If weight-stationary, this instruction will compute on the previously 
                // preloaded weights (B) in the systolic array
                auto A_scratchpad_addr = Extract(rs1, 31, 0);
                auto A_col = Extract(rs1, 47, 32);
                auto A_row = Extract(rs1, 63, 48);
                auto BD_scratchpad_addr = Extract(rs2, 31, 0);
                auto BD_col = Extract(rs2, 47, 32);
                auto BD_row = Extract(rs2, 63, 48);
            }
        }

        // Note: Loop instructions not included in README, therefore not implemented for now
    }

    

}