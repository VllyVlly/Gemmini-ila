#include "test_helpers.h"
#include "gemmini.h"
#include <iomanip>
#include <iostream>

namespace gemmini {

std::vector<TestResult> g_test_results;

thread_local int g_current_failures = 0;

void record_failure(const std::string& msg)
{
    std::cerr << " (!) FAIL: " << msg << std::endl;
    g_current_failures++;
}

// Helper to constrain an ILA state variable at a specific step
// Call AFTER unrolling, adds constraint directly to solver
// step defaults to step 0 (initial step)
// Bool
void cstr_step_bool(z3::solver& s, ilang::IlaZ3Unroller& u, z3::context& ctx, const ilang::ExprRef& ila_expr, bool value, int step)
{
    auto expr = u.GetZ3Expr(ila_expr, step);
    s.add(expr == ctx.bool_val(value));
}

// Z3 Int
void cstr_step_int(z3::solver& s, ilang::IlaZ3Unroller& u, z3::context& ctx, const ilang::ExprRef& ila_expr, int value, int step)
{
    auto expr = u.GetZ3Expr(ila_expr, step);
    s.add(expr == ctx.int_val(value));
}

// Bit Vector
// matches Z3's bv_val(uint64_t, unsigned) overload
void cstr_step_bv(z3::solver& s, ilang::IlaZ3Unroller& u, z3::context& ctx, const ilang::ExprRef& ila_expr, uint64_t value, size_t bit_width, int step)
{
    auto expr = u.GetZ3Expr(ila_expr, step);
    s.add(expr == ctx.bv_val(value, bit_width));
}

// Generic Z3 Expression
void cstr_step(z3::solver& s, ilang::IlaZ3Unroller& u, z3::context& ctx, const ilang::ExprRef& ila_expr, const z3::expr& value_expr, int step)
{
    auto expr = u.GetZ3Expr(ila_expr, step);
    s.add(expr == value_expr);
}

// Create a 128-bit Z3 expression from two 64-bit halves
z3::expr bv_val_128(z3::context& ctx, uint64_t high_half, uint64_t low_half)
{
    return z3::concat(ctx.bv_val(high_half, 64), ctx.bv_val(low_half, 64));
}

std::string TO_STR(const ilang::ExprRef& ila_expr, int step, ilang::IlaZ3Unroller& u, z3::model& mdl)
{
    auto expr = u.GetZ3Expr(ila_expr, step);
    auto eval = mdl.eval(expr);
    return eval.to_string();
}

std::string HexToDecimalString(const std::string& hex_in)
{
    std::string hex = hex_in;

    // Strip optional "#x" or "0x" prefix
    if (hex.size() >= 2 && hex[0] == '#' && (hex[1] == 'x' || hex[1] == 'X')) {
        hex = hex.substr(2);
    } else if (hex.size() >= 2 && hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X')) {
        hex = hex.substr(2);
    }

    // Strip leading zeros (keep at least one digit)
    size_t first_nonzero = hex.find_first_not_of('0');
    if (first_nonzero == std::string::npos) {
        return "0"; // all zeros
    }
    hex = hex.substr(first_nonzero);

    // Big-decimal accumulator, stored as vector of decimal digits (most significant first)
    std::vector<int> decimal_digits = { 0 };

    auto multiply_by_16_add_digit = [&](int digit_value) {
        int carry = digit_value;
        for (int i = static_cast<int>(decimal_digits.size()) - 1; i >= 0; --i) {
            int val = decimal_digits[i] * 16 + carry;
            decimal_digits[i] = val % 10;
            carry = val / 10;
        }
        while (carry > 0) {
            decimal_digits.insert(decimal_digits.begin(), carry % 10);
            carry /= 10;
        }
    };

    for (char c : hex) {
        int digit_value;
        if (c >= '0' && c <= '9')
            digit_value = c - '0';
        else if (c >= 'a' && c <= 'f')
            digit_value = c - 'a' + 10;
        else if (c >= 'A' && c <= 'F')
            digit_value = c - 'A' + 10;
        else
            continue; // skip any stray non-hex chars

        multiply_by_16_add_digit(digit_value);
    }

    std::string result;
    for (int d : decimal_digits)
        result += static_cast<char>('0' + d);
    return result;
}

void PRINT(const ilang::ExprRef& ila_expr, int step, ilang::IlaZ3Unroller& u, z3::model& mdl, std::string label)
{
    static size_t counter = 0;
    auto expr = u.GetZ3Expr(ila_expr, step);
    auto eval = mdl.eval(expr);
    std::cout << " LOG[" << counter++ << "] '" << label << "' " << eval.to_string() << std::endl;
}

void CHECK(const std::string& test_name, Gemmini& gem, const std::vector<std::string>& instr_names,
    std::function<void(ilang::IlaZ3Unroller&, z3::solver&, z3::context&)> setup_fn,
    std::function<void(z3::model&, ilang::IlaZ3Unroller&)> verify_fn)
{
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

void print_test_summary()
{
    std::cout << "\n\n===========================================" << std::endl;
    std::cout << "            TEST SUMMARY" << std::endl;
    std::cout << "===========================================" << std::endl;

    int passed = 0;
    int failed = 0;

    for (const auto& result : g_test_results) {
        std::cout << "  " << (result.passed ? "[PASS]" : "[FAIL]")
                  << " " << result.test_name << std::endl;
        if (result.passed)
            passed++;
        else
            failed++;
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
uint64_t build_mvin_rs2(uint32_t addr, uint16_t rows, uint16_t cols)
{
    uint64_t rs2 = 0;
    rs2 |= addr; // Bits 31:0
    rs2 |= (uint64_t)cols << 32; // Bits 47:32
    rs2 |= (uint64_t)rows << 48; // Bits 63:48
    return rs2;
}

// Build rs1 for matmul.preload: address + rows + columns
uint64_t build_preload_rs(uint32_t addr, uint16_t rows, uint16_t cols)
{
    uint64_t rs = 0;
    rs |= addr; // Bits 31:0
    rs |= (uint64_t)cols << 32; // Bits 47:32
    rs |= (uint64_t)rows << 48; // Bits 63:48
    return rs;
}

} // namespace gemmini