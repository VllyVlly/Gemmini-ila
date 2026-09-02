#include "gemmini.h"
#include "test_helpers.h"

using namespace ilang;
using namespace gemmini;

void test_matmul_preload(Gemmini& gem)
{
    CHECK("Preload systolic array of DIM 2x2", gem, { "config_ex", "matmul.preload" },

        [&](ilang::IlaZ3Unroller& u, z3::solver& s, z3::context& ctx) {
        uint64_t rs1_val = 0;
        rs1_val |= 0;                    // bits 1:0 = 00 for config_ex
        rs1_val |= (1ULL << 2);          // bit 2 = 1 (WS mode)
        rs1_val |= (0ULL << 3);          // bit 3 = 1 (ReLU off)
        rs1_val |= (0ULL << 8);          // bit 8 = 0 (A transpose off)
        rs1_val |= (0ULL << 9);          // bit 9 = 1 (B transpose off)
        rs1_val |= (1ULL << 16);         // bits 31:16 = 1 (A stride)
        rs1_val |= (0x3F800000ULL << 32); // bits 63:32 = 1.0f
        
        // rs2[31:0] = 8 (right shift)
        uint64_t rs2_val = 0;
        
        cstr_step_bv(s, u, ctx, gem.rs1, rs1_val, 64, 0);
        cstr_step_bv(s, u, ctx, gem.rs2, rs2_val, 64, 0);

        // Load data to scratchpad
        // Matrix B/D
        // 1 1
        // 2 3
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00000000), 0x0101, 16, 1); // row0: 1 1
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00000001), 0x0302, 16, 1); // row1: 2 3
        
        // Set B/D source address as 0x00000000, load 2 x 2
        cstr_step_bv(s, u, ctx, gem.rs1, build_preload_rs(0x00000000, 2, 2), 64, 1);
        // Set C destination address as 0x00001000
        cstr_step_bv(s, u, ctx, gem.rs2, build_preload_rs(0x00001000, 2, 2), 64, 1); },

        [&](z3::model& mdl, ilang::IlaZ3Unroller& u) {
            auto dest_addr = TO_STR(gem.dest_addr, 2, u, mdl);
            auto dest_row = HexToDecimalString(TO_STR(gem.dest_row, 2, u, mdl));
            auto dest_col = HexToDecimalString(TO_STR(gem.dest_col, 2, u, mdl));
            EXPECT_TRUE(dest_addr == "#x00001000");
            EXPECT_TRUE(dest_row == "2");
            EXPECT_TRUE(dest_col == "2");

            auto element1 = HexToDecimalString(TO_STR(gem.sys_array[0][0]->stationary_reg, 2, u, mdl));
            auto element2 = HexToDecimalString(TO_STR(gem.sys_array[0][1]->stationary_reg, 2, u, mdl));
            auto element3 = HexToDecimalString(TO_STR(gem.sys_array[1][0]->stationary_reg, 2, u, mdl));
            auto element4 = HexToDecimalString(TO_STR(gem.sys_array[1][1]->stationary_reg, 2, u, mdl));
            EXPECT_TRUE(element1 == "1");
            EXPECT_TRUE(element2 == "1");
            EXPECT_TRUE(element3 == "2");
            EXPECT_TRUE(element4 == "3"); });
}

void test_compute_preload_OS(Gemmini& gem)
{
    CHECK("Preload calculation of two arrays of DIM 2x2 in OS mode", gem, { "config_ex", "matmul.preload", "matmul.compute.preloaded", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step" },

        [&](ilang::IlaZ3Unroller& u, z3::solver& s, z3::context& ctx) {
            uint64_t rs1_val = 0;
            rs1_val |= 0; // bits 1:0 = 00 for config_ex
            rs1_val |= (0ULL << 2); // bit 2 = 0 (OS mode)
            rs1_val |= (0ULL << 3); // bit 3 = 0 (ReLU off)
            rs1_val |= (0ULL << 8); // bit 8 = 0 (A transpose off)
            rs1_val |= (0ULL << 9); // bit 9 = 0 (B transpose off)
            rs1_val |= (1ULL << 16); // bits 31:16 = 1 (A stride)
            rs1_val |= (0x3F800000ULL << 32); // bits 63:32 = 1.0f

            // rs2[31:0] = 0 (right shift)
            uint64_t rs2_val = 0;

            cstr_step_bv(s, u, ctx, gem.rs1, rs1_val, 64, 0);
            cstr_step_bv(s, u, ctx, gem.rs2, rs2_val, 64, 0);

            // Load data to scratchpad
            // Matrix B/D
            // 0 0
            // 0 0
            cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00000000), 0x0000, 16, 1); // row0: 0 0
            cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00000001), 0x0000, 16, 1); // row1: 0 0

            // Set D/B source address as 0x00000000, load 2 x 2
            cstr_step_bv(s, u, ctx, gem.rs1, build_preload_rs(0x00000000, 2, 2), 64, 1);
            // Set C destination address as 0x00001000
            cstr_step_bv(s, u, ctx, gem.rs2, build_preload_rs(0x00001000, 2, 2), 64, 1);

            // Load matrix A and B/D
            // Matrix A
            // 1 2
            // 2 1
            // Matrix B/D
            // 1 0
            // 1 0
            cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00002000), 0x0201, 16, 2); // row0: 1 2
            cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00002001), 0x0102, 16, 2); // row1: 2 1
            cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00003000), 0x0001, 16, 2); // row0: 1 0
            cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00003001), 0x0001, 16, 2); // row1: 1 0

            // Set operands
            cstr_step_bv(s, u, ctx, gem.rs1, build_preload_rs(0x00002000, 2, 2), 64, 2);
            cstr_step_bv(s, u, ctx, gem.rs2, build_preload_rs(0x00003000, 2, 2), 64, 2); },

        [&](z3::model& mdl, ilang::IlaZ3Unroller& u) {
        // Expect
        // 3 0
        // 3 0
        auto elem1 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001000), 7, 0), 7, u, mdl));
        auto elem2 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001000), 15, 8), 7, u, mdl));
        auto elem3 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001001), 7, 0), 7, u, mdl));
        auto elem4 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001001), 15, 8), 7, u, mdl));
        EXPECT_TRUE(elem1 == "3");
        EXPECT_TRUE(elem2 == "0");
        EXPECT_TRUE(elem3 == "3");
        EXPECT_TRUE(elem4 == "0"); });
}

void test_compute_preload_WS(Gemmini& gem)
{
    CHECK("Preload calculation of two arrays of DIM 2x2 in WS mode", gem, { "config_ex", "matmul.preload", "matmul.compute.preloaded", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step" },

        [&](ilang::IlaZ3Unroller& u, z3::solver& s, z3::context& ctx) {
            uint64_t rs1_val = 0;
            rs1_val |= 0; // bits 1:0 = 00 for config_ex
            rs1_val |= (1ULL << 2); // bit 2 = 1 (WS mode)
            rs1_val |= (0ULL << 3); // bit 3 = 0 (ReLU off)
            rs1_val |= (0ULL << 8); // bit 8 = 0 (A transpose off)
            rs1_val |= (0ULL << 9); // bit 9 = 0 (B transpose off)
            rs1_val |= (1ULL << 16); // bits 31:16 = 1 (A stride)
            rs1_val |= (0x3F800000ULL << 32); // bits 63:32 = 1.0f

            // rs2[31:0] = 0 (right shift)
            uint64_t rs2_val = 0;

            cstr_step_bv(s, u, ctx, gem.rs1, rs1_val, 64, 0);
            cstr_step_bv(s, u, ctx, gem.rs2, rs2_val, 64, 0);

            // Load data to scratchpad
            // Matrix B
            // 1 0
            // 1 0
            cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00000000), 0x0001, 16, 1); // row0: 1 0
            cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00000001), 0x0001, 16, 1); // row1: 1 0

            // Set B source address as 0x00000000, load 2 x 2
            cstr_step_bv(s, u, ctx, gem.rs1, build_preload_rs(0x00000000, 2, 2), 64, 1);
            // Set C destination address as 0x00001000
            cstr_step_bv(s, u, ctx, gem.rs2, build_preload_rs(0x00001000, 2, 2), 64, 1);

            // Load matrix A and D
            // Matrix A
            // 1 0
            // 2 0
            // Matrix D
            // 0 0
            // 0 0
            cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00002000), 0x0001, 16, 2); // row0: 1 0
            cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00002001), 0x0002, 16, 2); // row1: 2 0
            cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00003000), 0x0000, 16, 2); // row0: 0 0
            cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00003001), 0x0000, 16, 2); // row1: 0 0

            // Set operands
            cstr_step_bv(s, u, ctx, gem.rs1, build_preload_rs(0x00002000, 2, 2), 64, 2);
            cstr_step_bv(s, u, ctx, gem.rs2, build_preload_rs(0x00003000, 2, 2), 64, 2); },

        [&](z3::model& mdl, ilang::IlaZ3Unroller& u) {
        // Expect
        // 1 0
        // 2 0
        auto elem1 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001000), 7, 0), 7, u, mdl));
        auto elem2 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001000), 15, 8), 7, u, mdl));
        auto elem3 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001001), 7, 0), 7, u, mdl));
        auto elem4 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001001), 15, 8), 7, u, mdl));
        EXPECT_TRUE(elem1 == "1");
        EXPECT_TRUE(elem2 == "0");
        EXPECT_TRUE(elem3 == "2");
        EXPECT_TRUE(elem4 == "0"); });
}

void test_compute_preload_OS_A_transpose(Gemmini& gem)
{
    CHECK("Preload calculation OS 2x2 with A transpose", gem, { "config_ex", "matmul.preload", "matmul.compute.preloaded", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step" },

        [&](ilang::IlaZ3Unroller& u, z3::solver& s, z3::context& ctx) {
        uint64_t rs1_val = 0;
        rs1_val |= 0;                    // bits 1:0 = 00 for config_ex
        rs1_val |= (0ULL << 2);          // bit 2 = 0 (OS mode)
        rs1_val |= (0ULL << 3);          // bit 3 = 0 (ReLU off)
        rs1_val |= (1ULL << 8);          // bit 8 = 1 (A transpose ON)
        rs1_val |= (0ULL << 9);          // bit 9 = 0 (B transpose off)
        rs1_val |= (1ULL << 16);         // bits 31:16 = 1 (A stride)
        rs1_val |= (0x3F800000ULL << 32); // bits 63:32 = 1.0f

        uint64_t rs2_val = 0; // right shift

        cstr_step_bv(s, u, ctx, gem.rs1, rs1_val, 64, 0);
        cstr_step_bv(s, u, ctx, gem.rs2, rs2_val, 64, 0);

        // Preload D = 0 0 / 0 0
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00000000), 0x0000, 16, 1);
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00000001), 0x0000, 16, 1);

        cstr_step_bv(s, u, ctx, gem.rs1, build_preload_rs(0x00000000, 2, 2), 64, 1);
        cstr_step_bv(s, u, ctx, gem.rs2, build_preload_rs(0x00001000, 2, 2), 64, 1);

        // Matrix A (physical, pre-transpose)
        // 1 2
        // 3 0
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00002000), 0x0201, 16, 2); // row0: 1 2
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00002001), 0x0003, 16, 2); // row1: 3 0

        // Matrix B = identity
        // 1 0
        // 0 1
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00003000), 0x0001, 16, 2); // row0: 1 0
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00003001), 0x0100, 16, 2); // row1: 0 1

        cstr_step_bv(s, u, ctx, gem.rs1, build_preload_rs(0x00002000, 2, 2), 64, 2);
        cstr_step_bv(s, u, ctx, gem.rs2, build_preload_rs(0x00003000, 2, 2), 64, 2); },

        [&](z3::model& mdl, ilang::IlaZ3Unroller& u) {
        // Expect
        // 1 3
        // 2 0
        auto elem1 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001000), 7, 0), 7, u, mdl));
        auto elem2 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001000), 15, 8), 7, u, mdl));
        auto elem3 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001001), 7, 0), 7, u, mdl));
        auto elem4 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001001), 15, 8), 7, u, mdl));
        EXPECT_TRUE(elem1 == "1");
        EXPECT_TRUE(elem2 == "3");
        EXPECT_TRUE(elem3 == "2");
        EXPECT_TRUE(elem4 == "0"); });
}

void test_compute_preload_OS_B_transpose(Gemmini& gem)
{
    CHECK("Preload calculation OS 2x2 with B transpose", gem, { "config_ex", "matmul.preload", "matmul.compute.preloaded", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step" },

        [&](ilang::IlaZ3Unroller& u, z3::solver& s, z3::context& ctx) {
        uint64_t rs1_val = 0;
        rs1_val |= 0;
        rs1_val |= (0ULL << 2);          // OS mode
        rs1_val |= (0ULL << 3);          // ReLU off
        rs1_val |= (0ULL << 8);          // A transpose off
        rs1_val |= (1ULL << 9);          // B transpose ON
        rs1_val |= (1ULL << 16);         // A stride
        rs1_val |= (0x3F800000ULL << 32); // scale 1.0f

        uint64_t rs2_val = 0;

        cstr_step_bv(s, u, ctx, gem.rs1, rs1_val, 64, 0);
        cstr_step_bv(s, u, ctx, gem.rs2, rs2_val, 64, 0);

        // Preload D = 0 0 / 0 0
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00000000), 0x0000, 16, 1);
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00000001), 0x0000, 16, 1);

        cstr_step_bv(s, u, ctx, gem.rs1, build_preload_rs(0x00000000, 2, 2), 64, 1);
        cstr_step_bv(s, u, ctx, gem.rs2, build_preload_rs(0x00001000, 2, 2), 64, 1);

        // Matrix A = identity
        // 1 0
        // 0 1
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00002000), 0x0001, 16, 2);
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00002001), 0x0100, 16, 2);

        // Matrix B (physical, pre-transpose)
        // 1 0
        // 2 0
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00003000), 0x0001, 16, 2); // row0: 1 0
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00003001), 0x0002, 16, 2); // row1: 2 0

        cstr_step_bv(s, u, ctx, gem.rs1, build_preload_rs(0x00002000, 2, 2), 64, 2);
        cstr_step_bv(s, u, ctx, gem.rs2, build_preload_rs(0x00003000, 2, 2), 64, 2); },

        [&](z3::model& mdl, ilang::IlaZ3Unroller& u) {
        // Expect
        // 1 2
        // 0 0
        auto elem1 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001000), 7, 0), 7, u, mdl));
        auto elem2 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001000), 15, 8), 7, u, mdl));
        auto elem3 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001001), 7, 0), 7, u, mdl));
        auto elem4 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001001), 15, 8), 7, u, mdl));
        EXPECT_TRUE(elem1 == "1");
        EXPECT_TRUE(elem2 == "2");
        EXPECT_TRUE(elem3 == "0");
        EXPECT_TRUE(elem4 == "0"); });
}

void test_compute_preload_OS_AB_transpose(Gemmini& gem)
{
    CHECK("Preload calculation OS 2x2 with A and B transpose", gem, { "config_ex", "matmul.preload", "matmul.compute.preloaded", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step" },

        [&](ilang::IlaZ3Unroller& u, z3::solver& s, z3::context& ctx) {
        uint64_t rs1_val = 0;
        rs1_val |= 0;
        rs1_val |= (0ULL << 2);          // OS mode
        rs1_val |= (0ULL << 3);          // ReLU off
        rs1_val |= (1ULL << 8);          // A transpose ON
        rs1_val |= (1ULL << 9);          // B transpose ON
        rs1_val |= (1ULL << 16);         // A stride
        rs1_val |= (0x3F800000ULL << 32); // scale 1.0f

        uint64_t rs2_val = 0;

        cstr_step_bv(s, u, ctx, gem.rs1, rs1_val, 64, 0);
        cstr_step_bv(s, u, ctx, gem.rs2, rs2_val, 64, 0);

        // Preload D = 0 0 / 0 0
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00000000), 0x0000, 16, 1);
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00000001), 0x0000, 16, 1);

        cstr_step_bv(s, u, ctx, gem.rs1, build_preload_rs(0x00000000, 2, 2), 64, 1);
        cstr_step_bv(s, u, ctx, gem.rs2, build_preload_rs(0x00001000, 2, 2), 64, 1);

        // Matrix A (physical, pre-transpose)
        // 1 2
        // 3 0
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00002000), 0x0201, 16, 2);
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00002001), 0x0003, 16, 2);

        // Matrix B (physical, pre-transpose)
        // 1 0
        // 2 0
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00003000), 0x0001, 16, 2);
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00003001), 0x0002, 16, 2);

        cstr_step_bv(s, u, ctx, gem.rs1, build_preload_rs(0x00002000, 2, 2), 64, 2);
        cstr_step_bv(s, u, ctx, gem.rs2, build_preload_rs(0x00003000, 2, 2), 64, 2); },

        [&](z3::model& mdl, ilang::IlaZ3Unroller& u) {
        // Expect
        // 1 2
        // 2 4
        auto elem1 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001000), 7, 0), 7, u, mdl));
        auto elem2 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001000), 15, 8), 7, u, mdl));
        auto elem3 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001001), 7, 0), 7, u, mdl));
        auto elem4 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001001), 15, 8), 7, u, mdl));
        EXPECT_TRUE(elem1 == "1");
        EXPECT_TRUE(elem2 == "2");
        EXPECT_TRUE(elem3 == "2");
        EXPECT_TRUE(elem4 == "4"); });
}

void test_compute_preload_WS_A_transpose(Gemmini& gem)
{
    CHECK("Preload calculation WS 2x2 with A transpose", gem, { "config_ex", "matmul.preload", "matmul.compute.preloaded", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step" },

        [&](ilang::IlaZ3Unroller& u, z3::solver& s, z3::context& ctx) {
        uint64_t rs1_val = 0;
        rs1_val |= 0;
        rs1_val |= (1ULL << 2);          // WS mode
        rs1_val |= (0ULL << 3);          // ReLU off
        rs1_val |= (1ULL << 8);          // A transpose ON
        rs1_val |= (0ULL << 9);          // B transpose off
        rs1_val |= (1ULL << 16);         // A stride
        rs1_val |= (0x3F800000ULL << 32); // scale 1.0f

        uint64_t rs2_val = 0;

        cstr_step_bv(s, u, ctx, gem.rs1, rs1_val, 64, 0);
        cstr_step_bv(s, u, ctx, gem.rs2, rs2_val, 64, 0);

        // Matrix B (weight, preloaded stationary) = identity
        // 1 0
        // 0 1
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00000000), 0x0001, 16, 1);
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00000001), 0x0100, 16, 1);

        cstr_step_bv(s, u, ctx, gem.rs1, build_preload_rs(0x00000000, 2, 2), 64, 1);
        cstr_step_bv(s, u, ctx, gem.rs2, build_preload_rs(0x00001000, 2, 2), 64, 1);

        // Matrix A (physical, pre-transpose)
        // 1 2
        // 3 0
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00002000), 0x0201, 16, 2);
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00002001), 0x0003, 16, 2);

        // Matrix D = 0 0 / 0 0
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00003000), 0x0000, 16, 2);
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00003001), 0x0000, 16, 2);

        cstr_step_bv(s, u, ctx, gem.rs1, build_preload_rs(0x00002000, 2, 2), 64, 2);
        cstr_step_bv(s, u, ctx, gem.rs2, build_preload_rs(0x00003000, 2, 2), 64, 2); },

        [&](z3::model& mdl, ilang::IlaZ3Unroller& u) {
        // Expect
        // 1 3
        // 2 0
    
        auto elem1 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001000), 7, 0), 7, u, mdl));
        auto elem2 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001000), 15, 8), 7, u, mdl));
        auto elem3 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001001), 7, 0), 7, u, mdl));
        auto elem4 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001001), 15, 8), 7, u, mdl));
        EXPECT_TRUE(elem1 == "1");
        EXPECT_TRUE(elem2 == "3");
        EXPECT_TRUE(elem3 == "2");
        EXPECT_TRUE(elem4 == "0"); });
}

void test_compute_preload_WS_B_transpose(Gemmini& gem)
{
    CHECK("Preload calculation WS 2x2 with B transpose", gem, { "config_ex", "matmul.preload", "matmul.compute.preloaded", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step" },

        [&](ilang::IlaZ3Unroller& u, z3::solver& s, z3::context& ctx) {
        uint64_t rs1_val = 0;
        rs1_val |= 0;
        rs1_val |= (1ULL << 2);          // WS mode
        rs1_val |= (0ULL << 3);          // ReLU off
        rs1_val |= (0ULL << 8);          // A transpose off
        rs1_val |= (1ULL << 9);          // B transpose ON
        rs1_val |= (1ULL << 16);         // A stride
        rs1_val |= (0x3F800000ULL << 32); // scale 1.0f

        uint64_t rs2_val = 0;

        cstr_step_bv(s, u, ctx, gem.rs1, rs1_val, 64, 0);
        cstr_step_bv(s, u, ctx, gem.rs2, rs2_val, 64, 0);

        // Matrix B (weight, preloaded stationary, physical pre-transpose)
        // 1 0
        // 2 0
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00000000), 0x0001, 16, 1);
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00000001), 0x0002, 16, 1);

        cstr_step_bv(s, u, ctx, gem.rs1, build_preload_rs(0x00000000, 2, 2), 64, 1);
        cstr_step_bv(s, u, ctx, gem.rs2, build_preload_rs(0x00001000, 2, 2), 64, 1);

        // Matrix A = identity
        // 1 0
        // 0 1
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00002000), 0x0001, 16, 2);
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00002001), 0x0100, 16, 2);

        // Matrix D = 0 0 / 0 0
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00003000), 0x0000, 16, 2);
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00003001), 0x0000, 16, 2);

        cstr_step_bv(s, u, ctx, gem.rs1, build_preload_rs(0x00002000, 2, 2), 64, 2);
        cstr_step_bv(s, u, ctx, gem.rs2, build_preload_rs(0x00003000, 2, 2), 64, 2); },

        [&](z3::model& mdl, ilang::IlaZ3Unroller& u) {
        // Expect
        // 1 2
        // 0 0
        auto elem1 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001000), 7, 0), 7, u, mdl));
        auto elem2 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001000), 15, 8), 7, u, mdl));
        auto elem3 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001001), 7, 0), 7, u, mdl));
        auto elem4 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001001), 15, 8), 7, u, mdl));
        EXPECT_TRUE(elem1 == "1");
        EXPECT_TRUE(elem2 == "2");
        EXPECT_TRUE(elem3 == "0");
        EXPECT_TRUE(elem4 == "0"); });
}

void test_compute_preload_WS_AB_transpose(Gemmini& gem)
{
    CHECK("Preload calculation WS 2x2 with A and B transpose", gem, { "config_ex", "matmul.preload", "matmul.compute.preloaded", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step" },

        [&](ilang::IlaZ3Unroller& u, z3::solver& s, z3::context& ctx) {
        uint64_t rs1_val = 0;
        rs1_val |= 0;
        rs1_val |= (1ULL << 2);          // WS mode
        rs1_val |= (0ULL << 3);          // ReLU off
        rs1_val |= (1ULL << 8);          // A transpose ON
        rs1_val |= (1ULL << 9);          // B transpose ON
        rs1_val |= (1ULL << 16);         // A stride
        rs1_val |= (0x3F800000ULL << 32); // scale 1.0f

        uint64_t rs2_val = 0;

        cstr_step_bv(s, u, ctx, gem.rs1, rs1_val, 64, 0);
        cstr_step_bv(s, u, ctx, gem.rs2, rs2_val, 64, 0);

        // Matrix B (weight, preloaded stationary, physical pre-transpose)
        // 1 0
        // 2 0
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00000000), 0x0001, 16, 1);
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00000001), 0x0002, 16, 1);

        cstr_step_bv(s, u, ctx, gem.rs1, build_preload_rs(0x00000000, 2, 2), 64, 1);
        cstr_step_bv(s, u, ctx, gem.rs2, build_preload_rs(0x00001000, 2, 2), 64, 1);

        // Matrix A (physical, pre-transpose)
        // 1 2
        // 3 0
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00002000), 0x0201, 16, 2);
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00002001), 0x0003, 16, 2);

        // Matrix D = 0 0 / 0 0
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00003000), 0x0000, 16, 2);
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00003001), 0x0000, 16, 2);

        cstr_step_bv(s, u, ctx, gem.rs1, build_preload_rs(0x00002000, 2, 2), 64, 2);
        cstr_step_bv(s, u, ctx, gem.rs2, build_preload_rs(0x00003000, 2, 2), 64, 2); },

        [&](z3::model& mdl, ilang::IlaZ3Unroller& u) {
        // Expect
        // 1 2
        // 3 6
        
        auto elem1 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001000), 7, 0), 7, u, mdl));
        auto elem2 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001000), 15, 8), 7, u, mdl));
        auto elem3 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001001), 7, 0), 7, u, mdl));
        auto elem4 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001001), 15, 8), 7, u, mdl));
        EXPECT_TRUE(elem1 == "1");
        EXPECT_TRUE(elem2 == "2");
        EXPECT_TRUE(elem3 == "2");
        EXPECT_TRUE(elem4 == "4"); });
}

void test_compute_accumulate_OS(Gemmini& gem)
{
    CHECK("Accumulated calculation of two arrays of DIM 2x2", gem, { "config_ex", "matmul.preload", "matmul.compute.preloaded", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step", "matmul.compute.accumulated", "matmul.compute.accumulated_step", "matmul.compute.accumulated_step", "matmul.compute.accumulated_step", "matmul.compute.accumulated_step" },

        [&](ilang::IlaZ3Unroller& u, z3::solver& s, z3::context& ctx) {
        uint64_t rs1_val = 0;
        rs1_val |= 0;                    // bits 1:0 = 00 for config_ex
        rs1_val |= (0ULL << 2);          // bit 2 = 0 (OS mode)
        rs1_val |= (0ULL << 3);          // bit 3 = 0 (ReLU off)
        rs1_val |= (0ULL << 8);          // bit 8 = 0 (A transpose off)
        rs1_val |= (0ULL << 9);          // bit 9 = 0 (B transpose off)
        rs1_val |= (1ULL << 16);         // bits 31:16 = 1 (A stride)
        rs1_val |= (0x3F800000ULL << 32); // bits 63:32 = 1.0f
        
        // rs2[31:0] = 8 (right shift)
        uint64_t rs2_val = 0;
        
        cstr_step_bv(s, u, ctx, gem.rs1, rs1_val, 64, 0);
        cstr_step_bv(s, u, ctx, gem.rs2, rs2_val, 64, 0);

        // Load data to scratchpad
        // Matrix D
        // 0 0
        // 0 0
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00000000), 0x0000, 16, 1); // row0: 0 0
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00000001), 0x0000, 16, 1); // row1: 0 0
        
        // Set D source address as 0x00000000, load 2 x 2
        cstr_step_bv(s, u, ctx, gem.rs1, build_preload_rs(0x00000000, 2, 2), 64, 1);
        // Set C destination address as 0x00001000
        cstr_step_bv(s, u, ctx, gem.rs2, build_preload_rs(0x00001000, 2, 2), 64, 1);

        // Load matrix A and B
        // Matrix A
        // 1 2
        // 2 1
        // Matrix B
        // 1 0
        // 1 0
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00002000), 0x0201, 16, 2); // row0: 1 2
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00002001), 0x0102, 16, 2); // row1: 2 1
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00003000), 0x0001, 16, 2); // row0: 1 0
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00003001), 0x0001, 16, 2); // row1: 1 0

        // Set operands
        cstr_step_bv(s, u, ctx, gem.rs1, build_preload_rs(0x00002000, 2, 2), 64, 2);
        cstr_step_bv(s, u, ctx, gem.rs2, build_preload_rs(0x00003000, 2, 2), 64, 2);
    
        // Accumulate instruction
        cstr_step_bv(s, u, ctx, gem.rs1, build_preload_rs(0x00002000, 2, 2), 64, 7);
        cstr_step_bv(s, u, ctx, gem.rs2, build_preload_rs(0x00003000, 2, 2), 64, 7); },

        [&](z3::model& mdl, ilang::IlaZ3Unroller& u) {
        // Expect
        // 6 0
        // 6 0
        auto elem1 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001000), 7, 0), 12, u, mdl));
        auto elem2 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001000), 15, 8), 12, u, mdl));
        auto elem3 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001001), 7, 0), 12, u, mdl));
        auto elem4 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001001), 15, 8), 12, u, mdl));
        EXPECT_TRUE(elem1 == "6");
        EXPECT_TRUE(elem2 == "0");
        EXPECT_TRUE(elem3 == "6");
        EXPECT_TRUE(elem4 == "0"); });
}

void test_compute_accumulate_WS(Gemmini& gem)
{
    CHECK("Preload calculation of two arrays of DIM 2x2", gem, { "config_ex", "matmul.preload", "matmul.compute.preloaded", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step", "matmul.compute.accumulated", "matmul.compute.accumulated_step", "matmul.compute.accumulated_step", "matmul.compute.accumulated_step", "matmul.compute.accumulated_step" },

        [&](ilang::IlaZ3Unroller& u, z3::solver& s, z3::context& ctx) {
            uint64_t rs1_val = 0;
            rs1_val |= 0; // bits 1:0 = 00 for config_ex
            rs1_val |= (1ULL << 2); // bit 2 = 1 (WS mode)
            rs1_val |= (0ULL << 3); // bit 3 = 0 (ReLU off)
            rs1_val |= (0ULL << 8); // bit 8 = 0 (A transpose off)
            rs1_val |= (0ULL << 9); // bit 9 = 0 (B transpose off)
            rs1_val |= (1ULL << 16); // bits 31:16 = 1 (A stride)
            rs1_val |= (0x3F800000ULL << 32); // bits 63:32 = 1.0f

            // rs2[31:0] = 8 (right shift)
            uint64_t rs2_val = 0;

            cstr_step_bv(s, u, ctx, gem.rs1, rs1_val, 64, 0);
            cstr_step_bv(s, u, ctx, gem.rs2, rs2_val, 64, 0);

            // Load data to scratchpad
            // Matrix B
            // 1 0
            // 1 0
            cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00000000), 0x0001, 16, 1); // row0: 1 0
            cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00000001), 0x0001, 16, 1); // row1: 1 0

            // Set B source address as 0x00000000, load 2 x 2
            cstr_step_bv(s, u, ctx, gem.rs1, build_preload_rs(0x00000000, 2, 2), 64, 1);
            // Set C destination address as 0x00001000
            cstr_step_bv(s, u, ctx, gem.rs2, build_preload_rs(0x00001000, 2, 2), 64, 1);

            // Load matrix A and D
            // Matrix A
            // 1 2
            // 2 1
            // Matrix D
            // 0 0
            // 0 0
            cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00002000), 0x0201, 16, 2); // row0: 1 2
            cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00002001), 0x0102, 16, 2); // row1: 2 1
            cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00003000), 0x0000, 16, 2); // row0: 0 0
            cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00003001), 0x0000, 16, 2); // row1: 0 0

            // Set operands
            cstr_step_bv(s, u, ctx, gem.rs1, build_preload_rs(0x00002000, 2, 2), 64, 2);
            cstr_step_bv(s, u, ctx, gem.rs2, build_preload_rs(0x00003000, 2, 2), 64, 2);

            cstr_step_bv(s, u, ctx, gem.rs1, build_preload_rs(0x00002000, 2, 2), 64, 7);
            cstr_step_bv(s, u, ctx, gem.rs2, build_preload_rs(0x00003000, 2, 2), 64, 7); },

        [&](z3::model& mdl, ilang::IlaZ3Unroller& u) {
        // Expect
        // 3 0
        // 3 0
        auto elem1 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001000), 7, 0), 12, u, mdl));
        auto elem2 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001000), 15, 8), 12, u, mdl));
        auto elem3 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001001), 7, 0), 12, u, mdl));
        auto elem4 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001001), 15, 8), 12, u, mdl));
        EXPECT_TRUE(elem1 == "3");
        EXPECT_TRUE(elem2 == "0");
        EXPECT_TRUE(elem3 == "3");
        EXPECT_TRUE(elem4 == "0"); });
}

void test_compute_atomic_OS(Gemmini& gem)
{
    CHECK("Atomic compute of two arrays of DIM 2x2 in OS mode", gem, { "config_ex", "matmul.preload", "matmul.compute.atomic" },

        [&](ilang::IlaZ3Unroller& u, z3::solver& s, z3::context& ctx) {
            uint64_t rs1_val = 0;
            rs1_val |= 0; // bits 1:0 = 00 for config_ex
            rs1_val |= (0ULL << 2); // bit 2 = 0 (OS mode)
            rs1_val |= (0ULL << 3); // bit 3 = 0 (ReLU off)
            rs1_val |= (0ULL << 8); // bit 8 = 0 (A transpose off)
            rs1_val |= (0ULL << 9); // bit 9 = 0 (B transpose off)
            rs1_val |= (1ULL << 16); // bits 31:16 = 1 (A stride)
            rs1_val |= (0x3F800000ULL << 32); // bits 63:32 = 1.0f

            // rs2[31:0] = 0 (right shift)
            uint64_t rs2_val = 0;

            cstr_step_bv(s, u, ctx, gem.rs1, rs1_val, 64, 0);
            cstr_step_bv(s, u, ctx, gem.rs2, rs2_val, 64, 0);

            // Load data to scratchpad
            // Matrix B/D
            // 0 0
            // 0 0
            cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00000000), 0x0000, 16, 1); // row0: 0 0
            cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00000001), 0x0000, 16, 1); // row1: 0 0

            // Set D/B source address as 0x00000000, load 2 x 2
            cstr_step_bv(s, u, ctx, gem.rs1, build_preload_rs(0x00000000, 2, 2), 64, 1);
            // Set C destination address as 0x00001000
            cstr_step_bv(s, u, ctx, gem.rs2, build_preload_rs(0x00001000, 2, 2), 64, 1);

            // Load matrix A and B/D
            // Matrix A
            // 1 2
            // 2 1
            // Matrix B/D
            // 1 0
            // 1 0
            cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00002000), 0x0201, 16, 2); // row0: 1 2
            cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00002001), 0x0102, 16, 2); // row1: 2 1
            cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00003000), 0x0001, 16, 2); // row0: 1 0
            cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00003001), 0x0001, 16, 2); // row1: 1 0

            // Set operands: A source addr, B/D source addr, both 2x2
            cstr_step_bv(s, u, ctx, gem.rs1, build_preload_rs(0x00002000, 2, 2), 64, 2);
            cstr_step_bv(s, u, ctx, gem.rs2, build_preload_rs(0x00003000, 2, 2), 64, 2); },

        [&](z3::model& mdl, ilang::IlaZ3Unroller& u) {
        // Expect
        // 3 0
        // 3 0
        auto elem1 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001000), 7, 0), 3, u, mdl));
        auto elem2 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001000), 15, 8), 3, u, mdl));
        auto elem3 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001001), 7, 0), 3, u, mdl));
        auto elem4 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001001), 15, 8), 3, u, mdl));
        EXPECT_TRUE(elem1 == "3");
        EXPECT_TRUE(elem2 == "0");
        EXPECT_TRUE(elem3 == "3");
        EXPECT_TRUE(elem4 == "0"); });
}

void test_compute_atomic_WS(Gemmini& gem)
{
    CHECK("Atomic compute of two arrays of DIM 2x2 in WS mode", gem, { "config_ex", "matmul.preload", "matmul.compute.atomic" },

        [&](ilang::IlaZ3Unroller& u, z3::solver& s, z3::context& ctx) {
            uint64_t rs1_val = 0;
            rs1_val |= 0; // bits 1:0 = 00 for config_ex
            rs1_val |= (1ULL << 2); // bit 2 = 1 (WS mode)
            rs1_val |= (0ULL << 3); // bit 3 = 0 (ReLU off)
            rs1_val |= (0ULL << 8); // bit 8 = 0 (A transpose off)
            rs1_val |= (0ULL << 9); // bit 9 = 0 (B transpose off)
            rs1_val |= (1ULL << 16); // bits 31:16 = 1 (A stride)
            rs1_val |= (0x3F800000ULL << 32); // bits 63:32 = 1.0f

            // rs2[31:0] = 0 (right shift)
            uint64_t rs2_val = 0;

            cstr_step_bv(s, u, ctx, gem.rs1, rs1_val, 64, 0);
            cstr_step_bv(s, u, ctx, gem.rs2, rs2_val, 64, 0);

            // Load data to scratchpad
            // Matrix B
            // 1 0
            // 1 0
            cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00000000), 0x0001, 16, 1); // row0: 1 0
            cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00000001), 0x0001, 16, 1); // row1: 1 0

            // Set B source address as 0x00000000, load 2 x 2
            cstr_step_bv(s, u, ctx, gem.rs1, build_preload_rs(0x00000000, 2, 2), 64, 1);
            // Set C destination address as 0x00001000
            cstr_step_bv(s, u, ctx, gem.rs2, build_preload_rs(0x00001000, 2, 2), 64, 1);

            // Load matrix A and D
            // Matrix A
            // 1 0
            // 2 0
            // Matrix D
            // 0 0
            // 0 0
            cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00002000), 0x0001, 16, 2); // row0: 1 0
            cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00002001), 0x0002, 16, 2); // row1: 2 0
            cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00003000), 0x0000, 16, 2); // row0: 0 0
            cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00003001), 0x0000, 16, 2); // row1: 0 0

            // Set operands: A source addr, B/D source addr, both 2x2
            cstr_step_bv(s, u, ctx, gem.rs1, build_preload_rs(0x00002000, 2, 2), 64, 2);
            cstr_step_bv(s, u, ctx, gem.rs2, build_preload_rs(0x00003000, 2, 2), 64, 2); },

        [&](z3::model& mdl, ilang::IlaZ3Unroller& u) {
        // Expect
        // 1 0
        // 2 0
        auto elem1 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001000), 7, 0), 3, u, mdl));
        auto elem2 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001000), 15, 8), 3, u, mdl));
        auto elem3 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001001), 7, 0), 3, u, mdl));
        auto elem4 = HexToDecimalString(TO_STR(Extract(gem.scratchpad.Load(0x00001001), 15, 8), 3, u, mdl));
        EXPECT_TRUE(elem1 == "1");
        EXPECT_TRUE(elem2 == "0");
        EXPECT_TRUE(elem3 == "2");
        EXPECT_TRUE(elem4 == "0"); });
}

bool verifyComputeAtomicVsStepped(const gemmini::cfg& Cfg, int atomic_steps, int stepped_steps)
{

    std::cout << "[1] Building atomic and stepped ILA models...\n";

    // Build both models
    Gemmini atomic { Cfg };
    Gemmini stepped { Cfg };

    atomic.AddInstructions();
    stepped.AddInstructions();

    auto atomic_ila = atomic.get();
    auto stepped_ila = stepped.get();

    std::cout << "  Atomic:   " << atomic_ila.state_num() << " states, "
              << atomic_ila.instr_num() << " instrs\n";
    std::cout << "  Stepped:  " << stepped_ila.state_num() << " states, "
              << stepped_ila.instr_num() << " instrs\n";

    // Get key states for both models
    // Atomic model states

    auto rs1_atomic = atomic_ila.state("rs1");
    auto rs2_atomic = atomic_ila.state("rs2");
    auto rs1_stepped = stepped_ila.state("rs1");
    auto rs2_stepped = stepped_ila.state("rs2");

    auto a_atomic = atomic_ila.state("A_addr");
    auto b_atomic = atomic_ila.state("B_D_addr");
    auto dest_atomic = atomic_ila.state("dest_addr");
    auto A_row_atomic = atomic_ila.state("A_row");
    auto A_col_atomic = atomic_ila.state("A_col");
    auto BD_row_atomic = atomic_ila.state("B_D_row");
    auto BD_col_atomic = atomic_ila.state("B_D_col");
    auto scratchpad_atomic = atomic_ila.state("scratchpad");
    auto accumulator_atomic = atomic_ila.state("accumulator");
    auto done_atomic = atomic_ila.state("done");
    auto cmd_atomic = atomic_ila.input("funct");
    auto dataflow_atomic = atomic_ila.state("dataflow");
    auto A_T_atomic = atomic_ila.state("A_T");
    auto B_T_atomic = atomic_ila.state("B_T");
    auto right_shift_atomic = atomic_ila.state("right_shift");
    auto activation_func_atomic = atomic_ila.state("activation_func");
    auto A_stride_atomic = atomic_ila.state("A_stride");
    auto scalar_atomic = atomic_ila.state("scalar");
    auto acc_type_atomic = atomic_ila.state("acc_type");

    // Stepped model states
    auto a_stepped = stepped_ila.state("A_addr");
    auto b_stepped = stepped_ila.state("B_D_addr");
    auto dest_stepped = stepped_ila.state("dest_addr");
    auto A_row_stepped = stepped_ila.state("A_row");
    auto A_col_stepped = stepped_ila.state("A_col");
    auto BD_row_stepped = stepped_ila.state("B_D_row");
    auto BD_col_stepped = stepped_ila.state("B_D_col");
    auto scratchpad_stepped = stepped_ila.state("scratchpad");
    auto accumulator_stepped = stepped_ila.state("accumulator");
    auto done_stepped = stepped_ila.state("done");
    auto cmd_stepped = stepped_ila.input("funct");
    auto cycle_stepped = stepped_ila.state("cycle");
    auto dataflow_stepped = stepped_ila.state("dataflow");
    auto A_T_stepped = stepped_ila.state("A_T");
    auto B_T_stepped = stepped_ila.state("B_T");
    auto right_shift_stepped = stepped_ila.state("right_shift");
    auto activation_func_stepped = stepped_ila.state("activation_func");
    auto A_stride_stepped = stepped_ila.state("A_stride");
    auto scalar_stepped = stepped_ila.state("scalar");
    auto acc_type_stepped = stepped_ila.state("acc_type");


    // ----- 2. Unroll both models -----
    std::cout << "[2] Unrolling models...\n";
    z3::context c;
    IlaZ3Unroller unroller(c);

    // Atomic model: one instruction
    unroller.AddStepPred(0, cmd_atomic == BvConst(1, 3)); // compute command
    auto cstr_atomic = unroller.UnrollMonoConn(atomic_ila, atomic_steps);
    unroller.ClearInitPred();
    unroller.ClearStepPred();
    unroller.ClearGlobPred();

    // Stepped model: multiple steps
    unroller.AddStepPred(0, cmd_stepped == BvConst(4, 3)); // start compute
    for (int k = 1; k < stepped_steps; k++) {
        unroller.AddStepPred(k, cmd_stepped == BvConst(4, 3)); // stay in compute
    }
    auto cstr_stepped = unroller.UnrollMonoConn(stepped_ila, stepped_steps);
    unroller.ClearInitPred();
    unroller.ClearStepPred();
    unroller.ClearGlobPred();

    std::cout << "test1" << '\n';

    // ----- 3. Relate the two models (same inputs) -----
    std::cout << "[3] Adding equivalence constraints...\n";
    auto same_A = unroller.Equal(a_atomic, 0, a_stepped, 0);
    auto same_B = unroller.Equal(b_atomic, 0, b_stepped, 0);
    auto same_dest = unroller.Equal(dest_atomic, 0, dest_stepped, 0);
    auto same_A_row = unroller.Equal(A_row_atomic, 0, A_row_stepped, 0);
    auto same_A_col = unroller.Equal(A_col_atomic, 0, A_col_stepped, 0);
    auto same_BD_row = unroller.Equal(BD_row_atomic, 0, BD_row_stepped, 0);
    auto same_BD_col = unroller.Equal(BD_col_atomic, 0, BD_col_stepped, 0);
    auto same_dataflow = unroller.Equal(dataflow_atomic, 0, dataflow_stepped, 0);
    
    auto same_A_T = unroller.Equal(A_T_atomic, 0, A_T_stepped, 0);
    auto same_B_T = unroller.Equal(B_T_atomic, 0, B_T_stepped, 0);
    auto same_right_shift = unroller.Equal(right_shift_atomic, 0, right_shift_stepped, 0);
    auto same_activation = unroller.Equal(activation_func_atomic, 0, activation_func_stepped, 0);
    auto same_A_stride = unroller.Equal(A_stride_atomic, 0, A_stride_stepped, 0);
    auto same_scalar = unroller.Equal(scalar_atomic, 0, scalar_stepped, 0);
    auto same_acc_type = unroller.Equal(acc_type_atomic, 0, acc_type_stepped, 0);

    auto same_rs1 = unroller.Equal(rs1_atomic, 0, rs1_stepped, 0);
    auto same_rs2 = unroller.Equal(rs2_atomic, 0, rs2_stepped, 0);

    auto same_inputs = same_A && same_B && same_dest && 
                   same_A_row && same_A_col && 
                   same_BD_row && same_BD_col &&
                   same_dataflow && same_A_T && same_B_T &&
                   same_right_shift && same_activation &&
                   same_A_stride && same_scalar && same_acc_type && same_rs1 && same_rs2;

    // ----- 4. Check outputs match at final step -----
    auto eq_scratchpad = unroller.Equal(scratchpad_atomic, atomic_steps,
        scratchpad_stepped, stepped_steps);
    auto eq_accumulator = unroller.Equal(accumulator_atomic, atomic_steps,
        accumulator_stepped, stepped_steps);
    auto eq_done = unroller.Equal(done_atomic, atomic_steps,
        done_stepped, stepped_steps);
    auto agree = eq_scratchpad && eq_accumulator && eq_done;

    // ----- 5a. Non-vacuity check -----
    std::cout << "[4a] Checking non-vacuity (expect SAT)...\n";
    {
        z3::solver s(c);
        s.add(cstr_atomic);
        s.add(cstr_stepped);
        s.add(same_inputs);
        s.add(agree);
        auto r = s.check();
        std::cout << "  Non-vacuity (exists valid execution): " << r << "\n";
        if (r != z3::sat) {
            std::cout << "  WARNING: No valid execution found!\n";
        }
    }

    // ----- 5b. Equivalence check -----
    std::cout << "[4b] Checking equivalence (expect UNSAT)...\n";
    bool equivalent = false;
    {
        z3::solver s(c);
        s.add(cstr_atomic);
        s.add(cstr_stepped);
        s.add(same_inputs);
        s.add(!agree); // Negate agreement
        auto r = s.check();
        std::cout << "  Equivalence (no counterexample): " << r << "\n";
        equivalent = (r == z3::unsat);
    }

    std::cout << "\n=== RESULT ===\n";
    if (equivalent) {
        std::cout << "✅ PASS: Atomic and stepped models are EQUIVALENT\n";
    } else {
        std::cout << "❌ FAIL: Atomic and stepped models are NOT equivalent\n";
    }

    return equivalent;
}