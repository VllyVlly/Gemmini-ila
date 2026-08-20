#include "gemmini.h"

void test_config_ex(gemmini::Gemmini& gem);
void test_config_mvin(gemmini::Gemmini& gem);
void test_config_mvout(gemmini::Gemmini& gem);
void test_mvin(gemmini::Gemmini& gem);
void test_mvout(gemmini::Gemmini& gem);
void test_matmul_preload(gemmini::Gemmini& gem);
void test_compute_preload_OS(gemmini::Gemmini& gem);
void test_compute_preload_WS(gemmini::Gemmini& gem);
void test_compute_accumulate_OS(gemmini::Gemmini& gem);
void test_compute_accumulate_WS(gemmini::Gemmini& gem);
