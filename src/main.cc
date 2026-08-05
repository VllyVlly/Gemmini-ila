#include "gemmini.h"
#include "test_helpers.h"
#include "tests.h"
#include <iostream>

/** Multipurpose Main Function
 * to build the model and check that new instructions or states were added correctly
 * to call out to test scripts in tests/
 */

using namespace gemmini;

void list_instrs(Gemmini& gem) {
    Ila m = gem.get();
    
    std::cout << "\nInstructions created:\n";
    for (size_t i = 0; i < m.instr_num(); i++){
        std::cout << "  " << m.instr(i).name() << std::endl;
    }
    std::cout << std::endl;
}

void list_states(Gemmini& gem) {
    Ila m = gem.get();

    std::cout << "\nState variables:\n";
    for (size_t i = 0; i < m.state_num(); i++){
        auto state = m.state(i);
        std::cout << "  " << state.name() << " (" << state.bit_width() << " bits)" << std::endl;
    }
    std::cout << std::endl;
}

int main() {
    Gemmini gem{cfg()};

    gem.AddInstructions();
    
    // list everything
    list_states(gem);
    list_instrs(gem);
    
    // instruction unit tests
    test_config_ex(gem);
    test_config_mvin(gem);
    test_config_mvout(gem);

    test_mvin(gem);
    test_mvout(gem);
    
    // summary
    print_test_summary();

    return 0;
}