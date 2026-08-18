#include "test_helpers.h"
#include "gemmini.h"

using namespace ilang;
using namespace gemmini;

void test_matmul_preload(Gemmini& gem){
    CHECK("Preload systolic array of DIM 2x2", gem, {"config_ex", "matmul.preload"},
    
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
        cstr_step_bv(s, u, ctx, gem.rs2, build_preload_rs(0x00001000, 2, 2), 64, 1);
    }, 

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

void test_compute_preload(Gemmini& gem){
    CHECK("Preload calculation of two arrays of DIM 2x2", gem, {"config_ex", "matmul.preload", "matmul.compute.preloaded", "matmul.compute.preloaded_step", "matmul.compute.preloaded_step"},
    
    [&](ilang::IlaZ3Unroller& u, z3::solver& s, z3::context& ctx) {
        uint64_t rs1_val = 0;
        rs1_val |= 0;                    // bits 1:0 = 00 for config_ex
        rs1_val |= (0ULL << 2);          // bit 2 = 1 (OS mode)
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
        // TODO change when writing to dest_addr
        auto temp1 = HexToDecimalString(TO_STR(gem.sys_array[1][0]->A_reg, 3, u, mdl));
        auto temp2 = HexToDecimalString(TO_STR(gem.sys_array[1][0]->A_reg, 4, u, mdl));
        auto temp3 = HexToDecimalString(TO_STR(gem.sys_array[1][0]->A_reg, 5, u, mdl));
        std::cout << temp1 << '\n';
        std::cout << temp2 << '\n';
        std::cout << temp3 << '\n';
        auto temp4 = HexToDecimalString(TO_STR(gem.sys_array[1][0]->B_D_reg, 3, u, mdl));
        auto temp5 = HexToDecimalString(TO_STR(gem.sys_array[1][0]->B_D_reg, 4, u, mdl));
        auto temp6 = HexToDecimalString(TO_STR(gem.sys_array[1][0]->B_D_reg, 5, u, mdl));
        std::cout << temp4 << '\n';
        std::cout << temp5 << '\n';
        std::cout << temp6 << '\n';

        auto element1 = HexToDecimalString(TO_STR(gem.sys_array[0][0]->C_reg_out, 5, u, mdl));
        auto element2 = HexToDecimalString(TO_STR(gem.sys_array[0][1]->C_reg_out, 5, u, mdl));
        auto element3 = HexToDecimalString(TO_STR(gem.sys_array[1][0]->C_reg_out, 5, u, mdl));
        auto element4 = HexToDecimalString(TO_STR(gem.sys_array[1][1]->C_reg_out, 5, u, mdl));
        std::cout << element1 << '\n';
        std::cout << element2 << '\n';
        std::cout << element3 << '\n';
        std::cout << element4 << '\n';
        EXPECT_TRUE(element1 == "3");
        EXPECT_TRUE(element2 == "0");
        EXPECT_TRUE(element3 == "3");
        EXPECT_TRUE(element4 == "0");
    });

}

// void test_compute_accumulate(Gemmini& gem){
//     CHECK("Preload systolic array of DIM 2x2", gem, {"config_ex", "matmul.preload"},
    
//     [&](ilang::IlaZ3Unroller& u, z3::solver& s, z3::context& ctx) {
    
//     }, 

//     [&](z3::model& mdl, ilang::IlaZ3Unroller& u) {


        
//     });

// }