#include "gemmini.h"
#include "test_helpers.h"
#include "tests.h"
#include <iostream>

/** Multipurpose Main Function
 * to build the model and check that new instructions or states were added correctly
 * to call out to test scripts in tests/
 */

using namespace gemmini;

void list_instrs(Gemmini& gem)
{
    Ila m = gem.get();

    std::cout << "\nInstructions created:\n";
    for (size_t i = 0; i < m.instr_num(); i++) {
        std::cout << "  " << m.instr(i).name() << std::endl;
    }
    std::cout << std::endl;
}

void list_states(Gemmini& gem)
{
    Ila m = gem.get();

    std::cout << "\nState variables:\n";
    for (size_t i = 0; i < m.state_num(); i++) {
        auto state = m.state(i);
        std::cout << "  " << state.name() << " (" << state.bit_width() << " bits)" << std::endl;
    }
    std::cout << std::endl;
}

int main()
{
    cfg Config1 = cfg();
    cfg Config2 = cfg(2, 2, 1, 1, 4, 64 * 1024, 128 * 1024, DataType::INT8, DataType::INT16, DataType::INT32);

    Gemmini gem { Config1 };
    Gemmini gem_matmul { Config2 }; // 2x2 systolic array for convenience

    gem.AddInstructions();
    gem_matmul.AddInstructions();

    // List architectural states and instructions
    // list_states(gem);
    // list_instrs(gem);

    // Instruction unit tests
    // test_config_ex(gem);
    // test_config_mvin(gem);
    // test_config_mvout(gem);

    // test_mvin(gem);
    // test_mvout(gem);

    // test_matmul_preload(gem_matmul);
    test_compute_preload_OS(gem_matmul);
    test_compute_preload_WS(gem_matmul);
    // test_compute_accumulate_OS(gem_matmul);
    // test_compute_accumulate_WS(gem_matmul);

    test_compute_preload_OS_A_transpose(gem_matmul);
    test_compute_preload_OS_B_transpose(gem_matmul);
    test_compute_preload_OS_AB_transpose(gem_matmul);
    test_compute_preload_WS_A_transpose(gem_matmul);
    test_compute_preload_WS_B_transpose(gem_matmul);
    test_compute_preload_WS_AB_transpose(gem_matmul);

    // Summary
    print_test_summary();

    return 0;
}