#include "test_helpers.h"
#include "gemmini.h"

using namespace ilang;
using namespace gemmini;

void test_matmul_preload(Gemmini& gem){
    CHECK("Preload systolic array of DIM 2x2", gem, {"config_ex", "matmul.preload"},
    
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

        // Load data to scratchpad
        // Matrix B
        // 1 1
        // 2 3
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00000000), 0x0101, 16, 1); // row0: 1 1
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00000001), 0x0302, 16, 1); // row1: 2 3
        
        // Set B source address as 0x00000000, load 2 x 2
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

// void test_compute_preload(Gemmini& gem){
//     CHECK("Preload systolic array of DIM 2x2", gem, {"config_ex", "matmul.preload"},
    
//     [&](ilang::IlaZ3Unroller& u, z3::solver& s, z3::context& ctx) {
//         // Set rs1 with specific configuration values
//         // rs1[1:0] = 0 (type field for config_ex)
//         // rs1[2] = 1 (dataflow: WS mode)
//         // rs1[3] = 1 (activation: ReLU)
//         // rs1[8] = 0 (A transpose: off)
//         // rs1[9] = 1 (B transpose: on)
//         // rs1[31:16] = 4 (A stride)
//         // rs1[63:32] = 0x3F800000 (scalar = 1.0 as float32)
//         uint64_t rs1_val = 0;
//         rs1_val |= 0;                    // bits 1:0 = 00 for config_ex
//         rs1_val |= (1ULL << 2);          // bit 2 = 1 (WS mode)
//         rs1_val |= (1ULL << 3);          // bit 3 = 1 (ReLU)
//         rs1_val |= (0ULL << 8);          // bit 8 = 0 (A transpose off)
//         rs1_val |= (1ULL << 9);          // bit 9 = 1 (B transpose on)
//         rs1_val |= (4ULL << 16);         // bits 31:16 = 4 (A stride)
//         rs1_val |= (0x3F800000ULL << 32); // bits 63:32 = 1.0f
        
//         // rs2[31:0] = 8 (right shift)
//         uint64_t rs2_val = 8;
        
//         cstr_step_bv(s, u, ctx, gem.rs1, rs1_val, 64, 0);
//         cstr_step_bv(s, u, ctx, gem.rs2, rs2_val, 64, 0);

//         // Load data to scratchpad
//         // Matrix A
//         // 1 1
//         // 2 3
//         // Matrix B
//         // 2 2
//         // 3 3
//         cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00000000), 0x0101, 16, 1); // row0: 1 1
//         cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00000001), 0x0302, 16, 1); // row1: 2 3
//         cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00001001), 0x0202, 16, 1); // row0: 2 2
//         cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x00001002), 0x0303, 16, 1); // row1: 3 3

//         cstr_step_bv(s, u, ctx, gem.rs1, build_preload_rs(0x00000000, ), 64, 1);
//         // Set rs2 as scratchpad source address and 1 row, 1 column
//         cstr_step_bv(s, u, ctx, gem.rs2, build_mvin_rs2(0x2000, 1, 1), 64, 1);
//     }, 

//     [&](z3::model& mdl, ilang::IlaZ3Unroller& u) {


        
//     });

// }

// void test_compute_accumulate(Gemmini& gem){
//     CHECK("Preload systolic array of DIM 2x2", gem, {"config_ex", "matmul.preload"},
    
//     [&](ilang::IlaZ3Unroller& u, z3::solver& s, z3::context& ctx) {
    
//     }, 

//     [&](z3::model& mdl, ilang::IlaZ3Unroller& u) {


        
//     });

// }