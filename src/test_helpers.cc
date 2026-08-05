#include <iostream>
#include <iomanip>
#include "test_helpers.h"
#include "gemmini.h"

namespace gemmini {

std::vector<TestResult> g_test_results;

thread_local int g_current_failures = 0;

void record_failure(const std::string& msg) {
    std::cerr << " (!) FAIL: " << msg << std::endl;
    g_current_failures++;
}

// Helper to constrain an ILA state variable at a specific step
// Call AFTER unrolling, adds constraint directly to solver
// step defaults to step 0 (initial step)
// Bool
void cstr_step_bool(z3::solver &s, ilang::IlaZ3Unroller &u, z3::context &ctx, const ilang::ExprRef &ila_expr, bool value, int step) {
    auto expr = u.GetZ3Expr(ila_expr, step);
    s.add(expr == ctx.bool_val(value));
}

// Z3 Int
void cstr_step_int(z3::solver &s, ilang::IlaZ3Unroller &u, z3::context &ctx, const ilang::ExprRef &ila_expr, int value, int step) {
    auto expr = u.GetZ3Expr(ila_expr, step);
    s.add(expr == ctx.int_val(value));
}

// Bit Vector
// matches Z3's bv_val(uint64_t, unsigned) overload
void cstr_step_bv(z3::solver &s, ilang::IlaZ3Unroller &u, z3::context &ctx, const ilang::ExprRef &ila_expr, uint64_t value, size_t bit_width, int step) {
    auto expr = u.GetZ3Expr(ila_expr, step);
    s.add(expr == ctx.bv_val(value, bit_width));
}

// Generic Z3 Expression
void cstr_step(z3::solver &s, ilang::IlaZ3Unroller &u, z3::context &ctx, const ilang::ExprRef &ila_expr, const z3::expr &value_expr, int step) {
    auto expr = u.GetZ3Expr(ila_expr, step);
    s.add(expr == value_expr);
}


std::string TO_STR(const ilang::ExprRef &ila_expr, int step, ilang::IlaZ3Unroller &u, z3::model &mdl) {
    auto expr = u.GetZ3Expr(ila_expr, step);
    auto eval = mdl.eval(expr);
    return eval.to_string();
}

void PRINT(const ilang::ExprRef &ila_expr, int step, ilang::IlaZ3Unroller &u, z3::model &mdl, std::string label) {
    static size_t counter = 0;
    auto expr = u.GetZ3Expr(ila_expr, step);
    auto eval = mdl.eval(expr);
    std::cout << " LOG[" << counter++ << "] '" << label << "' "<< eval.to_string() << std::endl;
}

void CHECK(const std::string& test_name, Gemmini& gem, const std::vector<std::string>& instr_names,
           std::function<void(ilang::IlaZ3Unroller&, z3::solver&, z3::context&)> setup_fn,
           std::function<void(z3::model&, ilang::IlaZ3Unroller&)> verify_fn) {
    std::cout << "\n=== Test: " << test_name << " ===" << std::endl;
    bool test_passed = true;
    
    // reset failure count for this test
    g_current_failures = 0;
    
    try {
        ilang::Ila m = gem.get();
        
        // find instructions by name
        std::vector<ilang::InstrRef> instrs;
        for (const auto& name : instr_names) {
            bool found = false;
            for (size_t i = 0; i < m.instr_num(); i++) {
                if (m.instr(i).name() == name) {
                    instrs.push_back(m.instr(i));
                    found = true;
                    break;
                }
            }
            if (!found) {
                throw std::runtime_error("Instruction '" + name + "' not found");
            }
        }
        
        z3::context ctx;
        ilang::IlaZ3Unroller u(ctx);
        z3::solver s(ctx);
        
        // unroll the instruction path FIRST
        auto tr = u.UnrollPathConn(instrs, 0);
        s.add(tr);
        
        // call setup lambda to add constraints AFTER unrolling
        setup_fn(u, s, ctx);
        
        // set timeout (30 seconds)
        z3::params p(ctx);
        p.set("timeout", (unsigned)30000);
        s.set(p);
        
        // solve
        auto result = s.check();
        
        if (result == z3::sat) {
            // call verify lambda with the model
            auto mdl = s.get_model();
            verify_fn(mdl, u);
        } else if (result == z3::unsat) {
            record_failure("Solver returned UNSAT - no valid execution path");
        } else {
            record_failure("Solver returned UNKNOWN/timeout");
        }
        
        // check if any assertions failed
        if (g_current_failures > 0) {
            test_passed = false;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        test_passed = false;
    }
    
    // record result
    TestResult result;
    result.test_name = test_name;
    result.passed = test_passed;
    g_test_results.push_back(result);
    
    std::cout << "=== " << (test_passed ? "PASS" : "FAIL") << ": " << test_name << " ===" << std::endl;
}

void print_test_summary() {
    std::cout << "\n\n===========================================" << std::endl;
    std::cout << "            TEST SUMMARY" << std::endl;
    std::cout << "===========================================" << std::endl;
    
    int passed = 0;
    int failed = 0;
    
    for (const auto& result : g_test_results) {
        std::cout << "  " << (result.passed ? "[PASS]" : "[FAIL]") 
                  << " " << result.test_name << std::endl;
        if (result.passed) passed++;
        else failed++;
    }
    
    std::cout << "\n  Total: " << g_test_results.size() << " tests" << std::endl;
    std::cout << "  Passed: " << passed << std::endl;
    std::cout << "  Failed: " << failed << std::endl;
    
    if (failed == 0) {
        std::cout << "\n  All tests passed! 🎉" << std::endl;
    }
    
    std::cout << "===========================================" << std::endl;
}

// Gemmini Helper Functions

// Build rs2 for mvin/mvout: address + rows + columns
uint64_t build_mvin_rs2(uint32_t addr, uint16_t rows, uint16_t cols) {
    uint64_t rs2 = 0;
    rs2 |= addr;                          // Bits 31:0
    rs2 |= (uint64_t)cols << 32;          // Bits 47:32
    rs2 |= (uint64_t)rows << 48;          // Bits 63:48
    return rs2;
}

// Build rs1 for matmul.preload: address + rows + columns
uint64_t build_preload_rs1(uint32_t addr, uint16_t rows, uint16_t cols) {
    uint64_t rs1 = 0;
    rs1 |= addr;                          // Bits 31:0
    rs1 |= (uint64_t)cols << 32;          // Bits 47:32
    rs1 |= (uint64_t)rows << 48;          // Bits 63:48
    return rs1;
}

// Build rs2 for matmul.preload: address + rows + columns
uint64_t build_preload_rs2(uint32_t addr, uint16_t rows, uint16_t cols) {
    return build_preload_rs1(addr, rows, cols); // Same encoding
}

}  // namespace gemmini