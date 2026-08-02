#pragma once

#include <ilang/ilang++.h>
#include <bits/stdc++.h>

namespace gemmini{

    using namespace ilang;

    // TODO CHANGE LATER
    #define TEMP_BIT_WIDTH 64
    #define RS_WIDTH 64
    #define DEFAULT_DIM 16
    #define DIM_WIDTH 8

    constexpr int DRAM_ADDR_WIDTH = 32;
    constexpr int DRAM_DATA_WIDTH = 64;
    
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

    class Gemmini {

        private:

            Ila m;

            // Following are parameters

            // Systolic array dimensions 
            uint64_t tileRows;
            uint64_t tileColumns;
            uint64_t meshRows;
            uint64_t meshColumns;

            // Dataflow
            ExprRef dataflow;

            // Scratchpad and accumulator memory
            uint64_t sp_banks;
            uint64_t sp_capacity;
            uint64_t acc_capacity;

            // Type paramaters
            DataType inputType;
            DataType outputType;
            DataType accType;

            // Access-execute queue
            uint64_t ld_queue_length;
            uint64_t st_queue_length;
            uint64_t ex_queue_length;
            uint64_t rob_entries;

            // DMA
            uint64_t dma_maxbytes;
            uint64_t dma_buswidth;
            uint64_t mem_pipeline;

            // Scaling is optional and shall not be done for now for my sanity

            // Following are memory 
            ExprRef DRAM; // how do i model this lmao
            ExprRef scratchpad;
            ExprRef accumulator;
            std::vector<std::vector<PE>> sys_array;

            // Following are config states
            ExprRef shift;
            ExprRef A_stride;
            ExprRef scale; 
            ExprRef private_stride;
            ExprRef memory_stride_mvin;
            ExprRef memory_stride_mvout;

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
            // Note: maybe add another ExprRef for mode, but overlap with dataflow? 

            // Preload stuff
            ExprRef dest_addr;
            ExprRef dest_row; 
            ExprRef dest_col;

            // Following are input fields for decoding
            ExprRef funct;
            ExprRef rs1;
            ExprRef rs2;
            
        public:
            Gemmini();
            void AddInstructions();

            int DIM;

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

    enum class DataTypeKind : uint8_t {
        SINT = 0,
        UINT = 1,
        FLOAT = 2
    };

    struct DataType {
        DataTypeKind kind;
        uint64_t bit_width;
        
        // For float types only
        uint64_t exp_bits;
        uint64_t mantissa_bits;
        
        // Constructor for integer types
        DataType(DataTypeKind k, uint64_t bw) 
            : kind(k), bit_width(bw), exp_bits(0), mantissa_bits(0) {}
        
        // Constructor for float types
        DataType(uint64_t exp, uint64_t mantissa) 
            : kind(DataTypeKind::FLOAT), 
            bit_width(exp + mantissa + 1),
            exp_bits(exp), 
            mantissa_bits(mantissa) {}
        
        // Default constructor
        DataType() : kind(DataTypeKind::SINT), bit_width(0), 
                    exp_bits(0), mantissa_bits(0) {}
        
        // Convert to string for debugging
        std::string to_string() const {
            switch(kind) {
                case DataTypeKind::SINT: 
                    return "SInt(" + std::to_string(bit_width) + ".W)";
                case DataTypeKind::UINT: 
                    return "UInt(" + std::to_string(bit_width) + ".W)";
                case DataTypeKind::FLOAT: 
                    return "Float(" + std::to_string(exp_bits) + ", " + 
                        std::to_string(mantissa_bits) + ")";
                default: 
                    return "Unknown";
            }
        }
    };

    // Helper to create common types
    inline DataType SInt(uint64_t width) { return DataType(DataTypeKind::SINT, width); }
    inline DataType UInt(uint64_t width) { return DataType(DataTypeKind::UINT, width); }
    inline DataType Float(uint64_t exp, uint64_t mantissa) { 
        return DataType(exp, mantissa); 
    }

    // Systolic array is made up of DIM x DIM PE
    struct PE {
        ExprRef A_reg;          
        ExprRef B_reg;           
        ExprRef D_reg;          
        ExprRef C_reg;          
        ExprRef D_preload_reg; 
        
        // Not sure if needed
        // ExprRef preload_valid;  // Whether D_preload_reg holds valid data
        // ExprRef accumulate_mode; // 0 = preloaded, 1 = accumulated
        
        PE(Ila& m, size_t row, size_t col) {
            std::string prefix = "PE_" + std::to_string(row) + "_" + std::to_string(col);
            
            A_reg = m.NewBvState(prefix + "_A", INPUT_TYPE_BIT_WIDTH);
            B_reg = m.NewBvState(prefix + "_B", INPUT_TYPE_BIT_WIDTH);
            D_reg = m.NewBvState(prefix + "_D", INPUT_TYPE_BIT_WIDTH);
            D_preload_reg = m.NewBvState(prefix + "_D_preload", INPUT_TYPE_BIT_WIDTH);
            
            // C_reg bit width depends on mode
            size_t c_bits = (cfg.dataflow == "WS") ? OUTPUT_TYPE_BIT_WIDTH : ACC_TYPE_BIT_WIDTH;
            C_reg = m.NewBvState(prefix + "_C", c_bits);
            
            // Control flags
            // preload_valid = m.NewBvState(prefix + "_preload_valid", 1);
            // accumulate_mode = m.NewBvState(prefix + "_accumulate_mode", 1);
        }
    };

}