#ifndef SYSTEM_CONTROL_H
#define SYSTEM_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rte_atomic.h"

extern rte_atomic32_t keep_running;
extern rte_atomic32_t thread_num;

#ifdef __cplusplus
}
#endif

#endif

