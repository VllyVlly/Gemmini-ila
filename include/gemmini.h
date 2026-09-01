#pragma once

#include <bits/stdc++.h>
#include <ilang/ilang++.h>

using namespace ilang;

namespace gemmini {

#define DRAM_ADDR_WIDTH 64
#define DRAM_DATA_WIDTH 32
#define GEMMINI_ADDR_WIDTH 32
#define RS_WIDTH 64

#define SYMB_TRUE BoolConst(true)
#define SYMB_FALSE BoolConst(false)

// TODO CHANGE LATER
// For now use 8-bit ints
#define INPUT_TYPE_BIT_WIDTH 8
#define ACC_TYPE_BIT_WIDTH 32
#define OUTPUT_TYPE_BIT_WIDTH 16

// Pre-defined funct for convenience
#define mvin BvConst(2, 3)
#define mvout BvConst(3, 3)
#define config BvConst(0, 3)
#define matmul_preload BvConst(6, 3)
#define matmul_compute_preloaded BvConst(4, 3)
#define matmul_compute_accumulated BvConst(5, 3)

enum class DataType : uint64_t {
    INT8 = 8,
    INT16 = 16,
    INT32 = 32,
    FLOAT16 = 16,
    FLOAT32 = 32,
    // Add more as needed
};

// Configure Gemmini Paramaters
struct cfg {
    // Systolic array dimensions
    uint64_t tileRows;
    uint64_t tileColumns;
    uint64_t meshRows;
    uint64_t meshColumns;

    uint64_t DIM;

    // Scratchpad and accumulator memory
    uint64_t sp_banks;
    uint64_t sp_capacity;
    uint64_t acc_capacity;

    // Type paramaters
    DataType inputType;
    DataType outputType;
    DataType accType;

    cfg()
        : tileRows(16)
        , tileColumns(16)
        , meshRows(1)
        , meshColumns(1)
        , DIM(tileRows * meshRows)
        , sp_banks(4)
        , sp_capacity(64 * 1024)
        , // 64 KB
        acc_capacity(128 * 1024)
        , // 128 KB
        inputType(DataType::INT8)
        , outputType(DataType::INT16)
        , accType(DataType::INT32) { };

    cfg(
        uint64_t tileRows_,
        uint64_t tileColumns_,
        uint64_t meshRows_,
        uint64_t meshColumns_,
        uint64_t sp_banks_,
        uint64_t sp_capacity_,
        uint64_t acc_capacity_,
        DataType inputType_,
        DataType outputType_,
        DataType accType_)
        : tileRows(tileRows_)
        , tileColumns(tileColumns_)
        , meshRows(meshRows_)
        , meshColumns(meshColumns_)
        , DIM(tileRows * meshRows)
        , sp_banks(sp_banks_)
        , sp_capacity(sp_capacity_)
        , acc_capacity(acc_capacity_)
        , inputType(inputType_)
        , outputType(outputType_)
        , accType(accType_) { };
};

// Systolic array is made up of DIM x DIM PE
struct PE {
    ExprRef A_reg;
    ExprRef B_D_reg;
    ExprRef stationary_reg;
    ExprRef C_reg_out;

    PE(Ila& m, size_t row, size_t col)
        : A_reg(m.NewBvState("PE_" + std::to_string(row) + "_" + std::to_string(col) + "_A", INPUT_TYPE_BIT_WIDTH))
        , B_D_reg(m.NewBvState("PE_" + std::to_string(row) + "_" + std::to_string(col) + "_B_D", INPUT_TYPE_BIT_WIDTH))
        , stationary_reg(m.NewBvState("PE_" + std::to_string(row) + "_" + std::to_string(col) + "_stat", ACC_TYPE_BIT_WIDTH))
        , C_reg_out(m.NewBvState("PE_" + std::to_string(row) + "_" + std::to_string(col) + "_C_out", OUTPUT_TYPE_BIT_WIDTH)) { };
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

    ExprRef A_addr;
    ExprRef A_row;
    ExprRef A_col;
    ExprRef B_D_addr;
    ExprRef B_D_row;
    ExprRef B_D_col;

    ExprRef cycle;

    // Following are input fields for decoding
    ExprRef funct;
    ExprRef rs1;
    ExprRef rs2;

    Gemmini(cfg Cfg);
    void AddInstructions();

    Ila& get()
    {
        return m;
    }
};

// Helper Functions
inline ExprRef ResizeBv(const ExprRef& e, unsigned target_width)
{
    unsigned cur_width = e.bit_width();
    if (cur_width == target_width) {
        return e;
    } else if (cur_width < target_width) {
        return ZExt(e, target_width); // widening
    } else {
        return Extract(e, target_width - 1, 0); // narrowing (truncate high bits)
    }
}

inline uint64_t getBitWidth(DataType dataType)
{
    return static_cast<uint64_t>(dataType);
}

inline ExprRef Relu(const ExprRef& x)
{
    auto zero = BvConst(0, x.bit_width());
    auto is_neg = (x < zero);
    return Ite(is_neg, zero, x);
}

inline ExprRef ScaleBv(const ExprRef& acc_value, const ExprRef& acc_scale, unsigned input_bitwidth)
{
    ExprRef scaled = ResizeBv(acc_value * acc_scale, input_bitwidth);
    return scaled;
}

}

// TODO
// Test gap between each DRAM element
// Do README and add better comments
// Float multiplication
// Refresh states once instructions finish