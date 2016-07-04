#ifndef MUR_MUR_HASH_H
#define MUR_MUR_HASH_H

#include <stdint.h>

#define SEED	0xbc9f1d34

uint32_t murmur(const char *key,  uint32_t len);

#endif

