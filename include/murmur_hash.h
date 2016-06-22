#ifndef MUR_MUR_HASH_H
#define MUR_MUR_HASH_H

#include <stdint.h>

uint32_t murmur3_32(const char *key, uint32_t len, uint32_t seed);

#endif

