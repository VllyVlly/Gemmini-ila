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
        uint64_t rs2_val = 8;
        
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
            EXPECT_TRUE(element4 == "3");
        });
}

void test_compute_preload_OS(Gemmini& gem)
{
    CHECK("Preload calculation of two arrays of DIM 2x2", gem, { "config_ex", "matmul.preload", "matmul.compute.preloaded", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step" },

        [&](ilang::IlaZ3Unroller& u, z3::solver& s, z3::context& ctx) {
            uint64_t rs1_val = 0;
            rs1_val |= 0; // bits 1:0 = 00 for config_ex
            rs1_val |= (0ULL << 2); // bit 2 = 0 (OS mode)
            rs1_val |= (0ULL << 3); // bit 3 = 0 (ReLU off)
            rs1_val |= (0ULL << 8); // bit 8 = 0 (A transpose off)
            rs1_val |= (0ULL << 9); // bit 9 = 0 (B transpose off)
            rs1_val |= (1ULL << 16); // bits 31:16 = 1 (A stride)
            rs1_val |= (0x3F800000ULL << 32); // bits 63:32 = 1.0f

            // rs2[31:0] = 8 (right shift)
            uint64_t rs2_val = 8;

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
            cstr_step_bv(s, u, ctx, gem.rs2, build_preload_rs(0x00003000, 2, 2), 64, 2);
        },

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
    CHECK("Preload calculation of two arrays of DIM 2x2", gem, { "config_ex", "matmul.preload", "matmul.compute.preloaded", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step" },

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
            uint64_t rs2_val = 8;

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
            cstr_step_bv(s, u, ctx, gem.rs2, build_preload_rs(0x00003000, 2, 2), 64, 2);
        },

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

        uint64_t rs2_val = 8; // right shift

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

        uint64_t rs2_val = 8;

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

        uint64_t rs2_val = 8;

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

        uint64_t rs2_val = 8;

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

        uint64_t rs2_val = 8;

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

        uint64_t rs2_val = 8;

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
        uint64_t rs2_val = 8;
        
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
            uint64_t rs2_val = 8;

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
            cstr_step_bv(s, u, ctx, gem.rs2, build_preload_rs(0x00003000, 2, 2), 64, 7);
        },

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