#include "test_helpers.h"
#include "gemmini.h"

using namespace ilang;
using namespace gemmini;

void test_mvin(Gemmini& gem){
    CHECK("mvin moves from DRAM to scratchpad", gem, {"mvin"}, 
    [&](ilang::IlaZ3Unroller& u, z3::solver& s, z3::context& ctx) {
        // Set DRAM content at address 0x1000 to value 42
        cstr_step_bv(s, u, ctx, gem.DRAM.Load(0x1000), 42, 32, 0);
        
        // Set rs1 as DRAM address
        cstr_step_bv(s, u, ctx, gem.rs1, 0x1000, 64, 0);
        // Set scratchpad destination address as 0x2000, and move 1 row and 1 column
        cstr_step_bv(s, u, ctx, gem.rs2, build_mvin_rs2(0x2000, 1, 1), 64, 0);
    }, 

    [&](z3::model& mdl, ilang::IlaZ3Unroller& u) {
        auto result = TO_STR(gem.scratchpad.Load(0x2000), 1, u, mdl);
        EXPECT_TRUE(result == "42");
    },

    );
}

void test_mvout(Gemmini& gem){
    CHECK("mvout moves from scratchpad to DRAM", gem, {"mvout"}, 
    [&](ilang::IlaZ3Unroller& u, z3::solver& s, z3::context& ctx) {
        // Set scratchpad content at address 0x2000 to value 42
        cstr_step_bv(s, u, ctx, gem.scratchpad.Load(0x2000), 42, 32, 0);
        
        // Set rs1 as DRAM destination address
        cstr_step_bv(s, u, ctx, gem.rs1, 0x1000, 64, 0);
        // Set rs2 as scratchpad source address and 1 row, 1 column
        cstr_step_bv(s, u, ctx, gem.rs2, build_mvout_rs2(0x2000, 1, 1), 64, 0);
    }, 

    [&](z3::model& mdl, ilang::IlaZ3Unroller& u) {
        auto result = TO_STR(gem.DRAM.Load(0x1000), 1, u, mdl);
        EXPECT_TRUE(result == "42");
    });
}