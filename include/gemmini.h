#pragma once

#include <ilang/ilang++.h>
#include <bits/stdc++.h>

using namespace ilang;

namespace gemmini{

    
    #define RS_WIDTH 64
    #define SYMB_TRUE BoolConst(true)
    #define SYMB_FALSE BoolConst(false)

    #define DRAM_ADDR_WIDTH 64
    #define DRAM_DATA_WIDTH 32
    
    // TODO CHANGE LATER
    // For now use 8-bit ints
    #define INPUT_TYPE_BIT_WIDTH 8
    #define ACC_TYPE_BIT_WIDTH 32
    #define OUTPUT_TYPE_BIT_WIDTH 16

    // Pre-defined funct for convenience
    #define mvin BvConst(2,3)
    #define mvout BvConst(3,3)
    #define config BvConst(0,3)
    #define matmul_preload BvConst(6,3)
    #define matmul_compute_preloaded BvConst(4,3)
    #define matmul_compute_accumulated BvConst(5,3)
    

    enum class DataType : uint64_t {
        INT8 = 8,
        INT16 = 16,
        INT32 = 32,
        FLOAT16 = 16,
        FLOAT32 = 32,
        // Add more as needed
    };

    struct cfg {
        // Systolic array dimensions 
        uint64_t tileRows;
        uint64_t tileColumns;
        uint64_t meshRows;
        uint64_t meshColumns;

        uint64_t DIM;

        // Dataflow
        bool dataflow; // 0 for OS, 1 for WS
        // Note: Might have to remove this since might affect with verification with config
        // Thus might have to change how to assign C_reg bit widths

        // Scratchpad and accumulator memory
        uint64_t sp_banks;
        uint64_t sp_capacity;
        uint64_t acc_capacity;

        // Type paramaters
        DataType inputType;
        DataType outputType;
        DataType accType;

        cfg() 
        :tileRows(16),
        tileColumns(16),
        meshRows(1),
        meshColumns(1),
        DIM(tileRows*meshRows),
        dataflow(true),  // WS mode by default
        sp_banks(4),
        sp_capacity(64 * 1024),   // 64 KB
        acc_capacity(128 * 1024), // 128 KB
        inputType(DataType::INT8),
        outputType(DataType::INT16),
        accType(DataType::INT32)
        {};

        cfg(
            uint64_t tileRows_,
            uint64_t tileColumns_,
            uint64_t meshRows_,
            uint64_t meshColumns_,
            bool dataflow_,
            uint64_t sp_banks_,
            uint64_t sp_capacity_,
            uint64_t acc_capacity_,
            DataType inputType_,
            DataType outputType_,
            DataType accType_
        ) 
        :tileRows(tileRows_),
        tileColumns(tileColumns_),
        meshRows(meshRows_),
        meshColumns(meshColumns_),
        DIM(tileRows*meshRows),
        dataflow(dataflow_),
        sp_banks(sp_banks_),
        sp_capacity(sp_capacity_),
        acc_capacity(acc_capacity_),
        inputType(inputType_),
        outputType(outputType_),
        accType(accType_)
        {};

        static constexpr size_t input_bits = 8;
        static constexpr size_t acc_bits = 32;
        static constexpr size_t output_bits = 16;
    };


    // Systolic array is made up of DIM x DIM PE
    struct PE {            
        ExprRef A_reg;
        ExprRef B_reg;
        ExprRef stationary_reg;     
        ExprRef C_reg_out;  

        PE(Ila& m, size_t row, size_t col):
            A_reg(m.NewBvState("PE_" + std::to_string(row) + "_" + std::to_string(col) + "_A", INPUT_TYPE_BIT_WIDTH)),
            B_reg(m.NewBvState("PE_" + std::to_string(row) + "_" + std::to_string(col) + "_B", INPUT_TYPE_BIT_WIDTH)),
            stationary_reg(m.NewBvState("PE_" + std::to_string(row) + "_" + std::to_string(col) + "_stat", ACC_TYPE_BIT_WIDTH)),
            C_reg_out(m.NewBvState("PE_" + std::to_string(row) + "_" + std::to_string(col) + "_C_out", OUTPUT_TYPE_BIT_WIDTH))
            {};
    };


    class Gemmini {

            
        public:

            cfg _Cfg;

            Ila m;

            ExprRef dataflow; // change this mode later ig lmao

            // Following are memory 
            ExprRef DRAM; // how do i model this lmao
            ExprRef scratchpad;
            ExprRef accumulator;
            std::vector<std::vector<std::unique_ptr<PE>>> sys_array;

            // Following are config states
            ExprRef shift;
            ExprRef A_stride;
            ExprRef scale; 
            ExprRef private_stride;
            ExprRef memory_stride_mvin;
            ExprRef memory_stride_mvout;
            ExprRef right_shift;

            ExprRef activation_func;
            ExprRef A_T;
            ExprRef B_T;
            ExprRef scalar;

            ExprRef acc_type;
            ExprRef mvin_type;

            ExprRef max_pool_stride;
            ExprRef max_pool_window_size;
            ExprRef upper_zero_pad;
            ExprRef left_zero_pad;
            ExprRef out_dim;
            ExprRef pool_row;
            ExprRef pool_col;
            ExprRef unpool_row;
            ExprRef unpool_col;

            // mvin and mvout
            ExprRef mvin_DRAM_addr;
            ExprRef mvin_dest_addr;
            ExprRef mvin_col_num;
            ExprRef mvin_row_num;
            ExprRef mvin_destination;

            ExprRef mvout_DRAM_addr;
            ExprRef mvout_sour_addr;
            ExprRef mvout_col_num;
            ExprRef mvout_row_num;
            ExprRef mvout_source;

            ExprRef start_row;
            ExprRef start_chunk;
            ExprRef done;

            // matmul preload
            ExprRef dest_addr;
            ExprRef dest_row; 
            ExprRef dest_col;

            // Following are input fields for decoding
            ExprRef funct;
            ExprRef rs1;
            ExprRef rs2;


            Gemmini(cfg Cfg);
            void AddInstructions();

            int DIM;

            Ila& get(){
                return m;
            }

            
            
    };

}

// TODO 
// Add casting helper functions
// Model transposer, and controller modules?
// Finish data movement instructions first
// Fix the paramater bidwidths
// Maybe change DRAM to uninterpreted functions
// Test gap between each DRAM element
// Use DRAM bit width variables for readability
// Do README and add better comments
// Do ReLu
// Deal with A-stride
// Deal with rightshift
// Float multiplication