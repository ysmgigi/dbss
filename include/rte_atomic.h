
#ifndef _RTE_ATOMIC_H_
#define _RTE_ATOMIC_H_

/**
 * @file
 * Atomic Operations
 *
 * This file defines a generic API for atomic
 * operations. The implementation is architecture-specific.
 *
 * @see lib/librte_eal/common/include/i686/arch/rte_atomic.h
 * @see lib/librte_eal/common/include/x86_64/arch/rte_atomic.h
 *
 * - \#include <stdint.h>
 * - \#include <rte_atomic.h>
 */

/*
 * This file provides an API for atomic operations. These operations
 * are architecture-specific.
 *
 * Atomic 32
 * ---------
 *
 *   typedef <tbd> rte_atomic32_t;
 *   RTE_ATOMIC32_INIT(int32_t)
 *
 * Usual atomic functions:
 *
 *   int32_t rte_atomic32_read(rte_atomic32_t *);
 *   void rte_atomic32_set(rte_atomic32_t *, int32_t);
 *   void rte_atomic32_add(rte_atomic32_t *, int32_t);
 *   void rte_atomic32_sub(rte_atomic32_t *, int32_t);
 *   void rte_atomic32_inc(rte_atomic32_t *);
 *   void rte_atomic32_dec(rte_atomic32_t *);
 *
 *
 * Same as above, but return the value after add/sub:
 *
 *   int32_t rte_atomic32_add_return(rte_atomic32_t *, int32_t);
 *   int32_t rte_atomic32_sub_return(rte_atomic32_t *, int32_t);
 *
 *
 * Atomically decrements by 1 and returns true if the result is 0, or
 * false for all other cases.
 *
 *   int rte_atomic32_dec_and_test(rte_atomic32_t *);
 *
 *
 * Atomically increments by 1 and returns true if the result is 0, or
 * false for all other cases
 *
 *   int rte_atomic32_inc_and_test(rte_atomic32_t *);
 *
 *
 * Atomically test and set. If value is already set, return 0
 * (failed), else set to 1 and return 1 (success)
 *
 *   int rte_atomic32_test_and_set(rte_atomic32_t *);
 *
 *
 * Atomically set the value to 0
 *
 *   void rte_atomic32_clear(rte_atomic32_t *);
 *
 * Atomic 64
 * ---------
 *
 *   typedef <tbd> rte_atomic64_t;
 *   RTE_ATOMIC64_INIT(int64_t)
 *
 * Usual atomic functions:
 *
 *   int64_t rte_atomic64_read(rte_atomic64_t *);
 *   void rte_atomic64_set(rte_atomic64_t *, int64_t);
 *   void rte_atomic64_add(rte_atomic64_t *, int64_t);
 *   void rte_atomic64_sub(rte_atomic64_t *, int64_t);
 *   void rte_atomic64_inc(rte_atomic64_t *);
 *   void rte_atomic64_dec(rte_atomic64_t *);
 *
 *
 * Same as above, but return the value after add/sub:
 *
 *   int64_t rte_atomic64_add_return(rte_atomic64_t *, int64_t);
 *   int64_t rte_atomic64_sub_return(rte_atomic64_t *, int64_t);
 *
 *
 * Atomically decrements by 1 and returns true if the result is 0, or
 * false for all other cases.
 *
 *   int rte_atomic64_dec_and_test(rte_atomic64_t *);
 *
 *
 * Atomically increments by 1 and returns true if the result is 0, or
 * false for all other cases
 *
 *   int rte_atomic64_inc_and_test(rte_atomic64_t *);
 *
 *
 * Atomically test and set. If value is already set, return 0
 * (failed), else set to 1 and return 1 (success)
 *
 *   int rte_atomic64_test_and_set(rte_atomic64_t *);
 *
 *
 * Atomically set the value to 0
 *
 *   void rte_atomic64_clear(rte_atomic64_t *);
 *
 */

#include <stdint.h>

#define CACHE_LINE_SIZE  64
#define __atomic_cache_aligned __attribute__((__aligned__(CACHE_LINE_SIZE)))

static inline void
rte_atomic16_inc(volatile uint16_t *v)
{
	asm volatile("lock ; "
		     "incw %[cnt]"
		     : [cnt] "=m" (*v) /* output (0) */
		     : "m" (*v)        /* input (1) */
		     );                /* no clobber-list */
}

#ifdef ARCH_X86_32
#include "rte_atomic_i686.h"
#else
#include "rte_atomic_x64.h"
#endif

#endif /* _RTE_ATOMIC_H_ */
