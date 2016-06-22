#include "hash.h"

uint32_t hash(const char *key, uint32_t len, uint32_t seed) {
	return murmur_hash(key, len, seed);
}

