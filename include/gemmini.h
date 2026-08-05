#pragma once

#include <ilang/ilang++.h>
#include <bits/stdc++.h>

using namespace ilang;

namespace gemmini{

    // TODO CHANGE LATER
    #define RS_WIDTH 64

    #define SYMBOLIC_TRUE BvConst(0,1)
    #define SYMBOLIC_FALSE BvConst(0,2)

    inline bool checkCond(ExprRef condition){
        return (condition.bit_width() == 1) ? true : false;
    }

    constexpr int DRAM_ADDR_WIDTH = 32;
    constexpr int DRAM_DATA_WIDTH = 64;
    
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
    // Maybe loop instruction?

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

        uint64_t input_bits() const { return static_cast<uint64_t>(inputType); }
        uint64_t acc_bits() const { return static_cast<uint64_t>(accType); }
    };


    // Systolic array is made up of DIM x DIM PE
    struct PE {
        ExprRef A_reg;          
        ExprRef B_reg;           
        ExprRef D_reg;          
        ExprRef C_reg_out; 
        ExprRef C_reg_acc;      
        ExprRef D_preload_reg; 
        
        // Not sure if needed
        // ExprRef preload_valid;  // Whether D_preload_reg holds valid data
        // ExprRef accumulate_mode; // 0 = preloaded, 1 = accumulated

        PE(Ila& m, size_t row, size_t col) 
            :A_reg(m.NewBvState("PE_" + std::to_string(row) + "_" + std::to_string(col) + "_A", INPUT_TYPE_BIT_WIDTH)),
            B_reg(m.NewBvState("PE_" + std::to_string(row) + "_" + std::to_string(col) + "_B", INPUT_TYPE_BIT_WIDTH)),
            D_reg(m.NewBvState("PE_" + std::to_string(row) + "_" + std::to_string(col) + "_D", INPUT_TYPE_BIT_WIDTH)),
            C_reg_out(m.NewBvState("PE_" + std::to_string(row) + "_" + std::to_string(col) + "_C_out", OUTPUT_TYPE_BIT_WIDTH)),
            C_reg_acc(m.NewBvState("PE_" + std::to_string(row) + "_" + std::to_string(col) + "_C_acc", ACC_TYPE_BIT_WIDTH)),
            D_preload_reg(m.NewBvState("PE_" + std::to_string(row) + "_" + std::to_string(col) + "_D_preload", INPUT_TYPE_BIT_WIDTH))
            {};
        //   accumulate_mode(m.NewBoolState("PE_" + std::to_string(row) + "_" + std::to_string(col) + "_accumulate")),
        //   preload_valid(m.NewBoolState("PE_" + std::to_string(row) + "_" + std::to_string(col) + "_preload_valid"))
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

            // Preload stuff
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

            // Helper functions

            // TODO 
            // Add casting helper functions
            // Model transposer, and controller modules?
            // Finish data movement instructions first
            // Make tests
            // Fix the paramater bidwidths
            // Maybe change DRAM to uninterpreted functions
            // Handle different mvin types and configuration registers?
            
    };

}