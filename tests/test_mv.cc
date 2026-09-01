#include "gemmini.h"
#include "test_helpers.h"

using namespace ilang;
using namespace gemmini;

void test_mvin(Gemmini& gem)
{
    CHECK("mvin moves from DRAM to scratchpad", gem, { "config_mvin", "mvin", "mvin_step" }, [&](ilang::IlaZ3Unroller& u, z3::solver& s, z3::context& ctx) {
        
        // Set configs first
        uint64_t rs1_val = 0;
        rs1_val |= 1;                    // bits 1:0 = 01 for config_mvin
        rs1_val |= (1ULL << 2);          // bit 2 = 1 (inputType to accumulator) 
        rs1_val |= (0ULL << 3);          // bits 4:3 = 0 (configure mvin)
        rs1_val |= (32ULL << 16);        // bits 31:16 = 32 (private stride)
        rs1_val |= (0x3F800000ULL << 32); // bits 63:32 = 1.0f
        uint64_t rs2_val = 64;
        cstr_step_bv(s, u, ctx, gem.rs1, rs1_val, 64, 0);
        cstr_step_bv(s, u, ctx, gem.rs2, rs2_val, 64, 0);

        // Set up memory
        // Set DRAM content at address 0x1000 to value 42
        cstr_step_bv(s, u, ctx, gem.DRAM.Load(0x0000000000001000), 42, 32, 1);
        // Set rs1 as DRAM address
        cstr_step_bv(s, u, ctx, gem.rs1, 0x0000000000001000, 64, 1);
        // Set scratchpad destination address as 0x2000, and move 1 row and 1 column
        cstr_step_bv(s, u, ctx, gem.rs2, build_mvin_rs2(0x2000, 1, 1), 64, 1); },

        [&](z3::model& mdl, ilang::IlaZ3Unroller& u) {
        auto result = HexToDecimalString(TO_STR(gem.scratchpad.Load(0x00002000), 3, u, mdl));
        EXPECT_TRUE(result == "42"); });
}

void test_mvout(Gemmini& gem)
{
    CHECK("mvout moves from scratchpad to DRAM", gem, { "config_mvout", "mvout", "mvout_step" }, [&](ilang::IlaZ3Unroller& u, z3::solver& s, z3::context& ctx) {

        // Set configs first
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

        // Set up memory
        // Set scratchpad content at address 0x2000 to value 42
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00002000), 42, 128, 1);
        
        // Set rs1 as DRAM destination address
        cstr_step_bv(s, u, ctx, gem.rs1, 0x0000000000001000, 64, 1);
        // Set rs2 as scratchpad source address and 1 row, 1 column
        cstr_step_bv(s, u, ctx, gem.rs2, build_mvin_rs2(0x2000, 1, 1), 64, 1); },

        [&](z3::model& mdl, ilang::IlaZ3Unroller& u) {
        auto result = HexToDecimalString(TO_STR(gem.DRAM.Load(0x0000000000001000), 3, u, mdl));
        EXPECT_TRUE(result == "42"); });
}