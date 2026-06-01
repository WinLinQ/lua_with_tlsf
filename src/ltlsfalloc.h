#ifndef ltlsfalloc_h
#define ltlsfalloc_h

#include <stddef.h>

void *lua_tlsf_alloc(void *ud, void *ptr, size_t osize, size_t nsize);

#endif
