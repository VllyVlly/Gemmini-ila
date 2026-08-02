#include "../include/test_helpers.h"
#include "../include/Gemmini.h"

using namespace ilang;
using namespace gemmini;

void test_config_ex(Gemmini& gem){
    CHECK("config_ex sets execute pipeline parameters", gem, {"config_ex"}, 

    [&](ilang::IlaZ3Unroller& u, z3::solver& s, z3::context& ctx) {
        // Set rs1 with specific configuration values
        // rs1[1:0] = 0 (type field for config_ex)
        // rs1[2] = 1 (dataflow: WS mode)
        // rs1[3] = 1 (activation: ReLU)
        // rs1[8] = 0 (A transpose: off)
        // rs1[9] = 1 (B transpose: on)
        // rs1[31:16] = 4 (A stride)
        // rs1[63:32] = 0x3F800000 (scalar = 1.0 as float32)
        uint64_t rs1_val = 0;
        rs1_val |= 0;                    // bits 1:0 = 00 for config_ex
        rs1_val |= (1ULL << 2);          // bit 2 = 1 (WS mode)
        rs1_val |= (1ULL << 3);          // bit 3 = 1 (ReLU)
        rs1_val |= (0ULL << 8);          // bit 8 = 0 (A transpose off)
        rs1_val |= (1ULL << 9);          // bit 9 = 1 (B transpose on)
        rs1_val |= (4ULL << 16);         // bits 31:16 = 4 (A stride)
        rs1_val |= (0x3F800000ULL << 32); // bits 63:32 = 1.0f
        
        // rs2[31:0] = 8 (right shift)
        uint64_t rs2_val = 8;
        
        cstr_step_bv(s, u, ctx, gem.rs1, rs1_val, 64, 0);
        cstr_step_bv(s, u, ctx, gem.rs2, rs2_val, 64, 0);
    }, 

    [&](z3::model& mdl, ilang::IlaZ3Unroller& u) {
        // Verify all state updates at step 1
        auto dataflow = TO_STR(gem.dataflow, 1, u, mdl);
        EXPECT_EQ_UINT(std::stoull(dataflow), 1);
        
        auto activation = TO_STR(gem.activation_func, 1, u, mdl);
        EXPECT_EQ_UINT(std::stoull(activation), 1);
        
        auto A_T = TO_STR(gem.A_T, 1, u, mdl);
        EXPECT_EQ_UINT(std::stoull(A_T), 0);
        
        auto B_T = TO_STR(gem.B_T, 1, u, mdl);
        EXPECT_EQ_UINT(std::stoull(B_T), 1);
        
        auto A_stride = TO_STR(gem.A_stride, 1, u, mdl);
        EXPECT_EQ_UINT(std::stoull(A_stride), 4);
        
        auto scalar = TO_STR(gem.scalar, 1, u, mdl);
        EXPECT_EQ_UINT(std::stoull(scalar), 0x3F800000);
        
        auto right_shift = TO_STR(gem.right_shift, 1, u, mdl);
        EXPECT_EQ_UINT(std::stoull(right_shift), 8);
    });
}

void test_config_mvin(Gemmini& gem){
    CHECK("config_mvin sets load pipeline parameters", gem, {"config_mvin"}, 

    [&](ilang::IlaZ3Unroller& u, z3::solver& s, z3::context& ctx) {
        // Set rs1 with specific configuration values
        // rs1[1:0] = 1 (type field for config_mvin)
        // rs1[2] = 1 (acc_type: inputType for mvin to accumulator)
        // rs1[4:3] = 0 (configure mvin, not mvin2 or mvin3)
        // rs1[31:16] = 32 (private stride)
        // rs1[63:32] = 0x3F800000 (scale = 1.0f)
        uint64_t rs1_val = 0;
        rs1_val |= 1;                    // bits 1:0 = 01 for config_mvin
        rs1_val |= (1ULL << 2);          // bit 2 = 1 (inputType to accumulator)
        rs1_val |= (0ULL << 3);          // bits 4:3 = 0 (configure mvin)
        rs1_val |= (32ULL << 16);        // bits 31:16 = 32 (private stride)
        rs1_val |= (0x3F800000ULL << 32); // bits 63:32 = 1.0f
        
        // rs2[63:0] = 64 (memory stride for mvin in bytes)
        uint64_t rs2_val = 64;
        
        cstr_step_bv(s, u, ctx, gem.rs1, rs1_val, 64, 0);
        cstr_step_bv(s, u, ctx, gem.rs2, rs2_val, 64, 0);
    }, 

    [&](z3::model& mdl, ilang::IlaZ3Unroller& u) {
        // Verify config_mvin state updates at step 1
        auto acc_type = TO_STR(gem.acc_type, 1, u, mdl);
        EXPECT_EQ_UINT(std::stoull(acc_type), 1);
        
        auto mvin_type = TO_STR(gem.mvin_type, 1, u, mdl);
        EXPECT_EQ_UINT(std::stoull(mvin_type), 0);
        
        auto private_stride = TO_STR(gem.private_stride, 1, u, mdl);
        EXPECT_EQ_UINT(std::stoull(private_stride), 32);
        
        auto mem_stride_mvin = TO_STR(gem.memory_stride_mvin, 1, u, mdl);
        EXPECT_EQ_UINT(std::stoull(mem_stride_mvin), 64);
        
        auto scale = TO_STR(gem.scale, 1, u, mdl);
        EXPECT_EQ_UINT(std::stoull(scale), 0x3F800000);
    });
}

void test_config_mvout(Gemmini& gem){
    CHECK("config_mvout sets store pipeline parameters", gem, {"config_mvout"}, 

    [&](ilang::IlaZ3Unroller& u, z3::solver& s, z3::context& ctx) {
        // Set rs1 with specific configuration values
        // rs1[1:0] = 2 (type field for config_mvout)
        // rs1[5:4] = 1 (max_pool_stride)
        // rs1[7:6] = 2 (max_pool_window_size)
        // rs1[9:8] = 1 (upper_zero_pad)
        // rs1[11:10] = 1 (left_zero_pad)
        // rs1[31:24] = 28 (out_dim)
        // rs1[39:32] = 4 (pool_row)
        // rs1[47:40] = 4 (pool_col)
        // rs1[55:48] = 8 (unpool_row)
        // rs1[63:56] = 8 (unpool_col)
        uint64_t rs1_val = 0;
        rs1_val |= 2;                    // bits 1:0 = 10 for config_mvout
        rs1_val |= (1ULL << 4);          // bits 5:4 = 1 (max_pool_stride)
        rs1_val |= (2ULL << 6);          // bits 7:6 = 2 (max_pool_window_size)
        rs1_val |= (1ULL << 8);          // bits 9:8 = 1 (upper_zero_pad)
        rs1_val |= (1ULL << 10);         // bits 11:10 = 1 (left_zero_pad)
        rs1_val |= (28ULL << 24);        // bits 31:24 = 28 (out_dim)
        rs1_val |= (4ULL << 32);         // bits 39:32 = 4 (pool_row)
        rs1_val |= (4ULL << 40);         // bits 47:40 = 4 (pool_col)
        rs1_val |= (8ULL << 48);         // bits 55:48 = 8 (unpool_row)
        rs1_val |= (8ULL << 56);         // bits 63:56 = 8 (unpool_col)
        
        // rs2[63:0] = 128 (memory stride for mvout in bytes)
        uint64_t rs2_val = 128;
        
        cstr_step_bv(s, u, ctx, gem.rs1, rs1_val, 64, 0);
        cstr_step_bv(s, u, ctx, gem.rs2, rs2_val, 64, 0);
    }, 

    [&](z3::model& mdl, ilang::IlaZ3Unroller& u) {
        // Verify config_mvout state updates at step 1
        auto max_pool_stride = TO_STR(gem.max_pool_stride, 1, u, mdl);
        EXPECT_EQ_UINT(std::stoull(max_pool_stride), 1);
        
        auto max_pool_window = TO_STR(gem.max_pool_window_size, 1, u, mdl);
        EXPECT_EQ_UINT(std::stoull(max_pool_window), 2);
        
        auto upper_zero_pad = TO_STR(gem.upper_zero_pad, 1, u, mdl);
        EXPECT_EQ_UINT(std::stoull(upper_zero_pad), 1);
        
        auto left_zero_pad = TO_STR(gem.left_zero_pad, 1, u, mdl);
        EXPECT_EQ_UINT(std::stoull(left_zero_pad), 1);
        
        auto out_dim = TO_STR(gem.out_dim, 1, u, mdl);
        EXPECT_EQ_UINT(std::stoull(out_dim), 28);
        
        auto pool_row = TO_STR(gem.pool_row, 1, u, mdl);
        EXPECT_EQ_UINT(std::stoull(pool_row), 4);
        
        auto pool_col = TO_STR(gem.pool_col, 1, u, mdl);
        EXPECT_EQ_UINT(std::stoull(pool_col), 4);
        
        auto unpool_row = TO_STR(gem.unpool_row, 1, u, mdl);
        EXPECT_EQ_UINT(std::stoull(unpool_row), 8);
        
        auto unpool_col = TO_STR(gem.unpool_col, 1, u, mdl);
        EXPECT_EQ_UINT(std::stoull(unpool_col), 8);
        
        auto mem_stride_mvout = TO_STR(gem.memory_stride_mvout, 1, u, mdl);
        EXPECT_EQ_UINT(std::stoull(mem_stride_mvout), 128);
        
    });
}