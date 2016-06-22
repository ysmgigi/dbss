#ifndef HASH_H
#define HASH_H

#include "murmur_hash.h"

uint32_t hash(const char *key, uint32_t len, uint32_t seed);

#endif

