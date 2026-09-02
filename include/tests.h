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

void test_compute_preload_OS_A_transpose(gemmini::Gemmini& gem);
void test_compute_preload_OS_B_transpose(gemmini::Gemmini& gem);
void test_compute_preload_OS_AB_transpose(gemmini::Gemmini& gem);
void test_compute_preload_WS_A_transpose(gemmini::Gemmini& gem);
void test_compute_preload_WS_B_transpose(gemmini::Gemmini& gem);
void test_compute_preload_WS_AB_transpose(gemmini::Gemmini& gem);

void test_compute_atomic_OS(gemmini::Gemmini& gem);
void test_compute_atomic_WS(gemmini::Gemmini& gem);