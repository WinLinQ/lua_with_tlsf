#include "ltlsfalloc.h"

#include <stdlib.h>
#include <sys/mman.h>

#include "../tlsf/tlsf.h"

#if !defined(LUA_TLSF_POOL_SIZE)
#define LUA_TLSF_POOL_SIZE	((size_t)128 * 1024 * 1024)
#endif

#if !defined(MAP_ANONYMOUS) && defined(MAP_ANON)
#define MAP_ANONYMOUS	MAP_ANON
#endif

#if !defined(MAP_POPULATE)
#error "MAP_POPULATE is required for the TLSF-backed Lua allocator"
#endif

static tlsf_t tlsf_state = NULL;
static void *tlsf_mapping = NULL;
static size_t tlsf_mapping_size = 0;


static void tlsf_allocator_cleanup(void) {
  if (tlsf_mapping != NULL) {
    munmap(tlsf_mapping, tlsf_mapping_size);
    tlsf_state = NULL;
    tlsf_mapping = NULL;
    tlsf_mapping_size = 0;
  }
}


static int tlsf_allocator_init(void) {
  void *mem;
  if (tlsf_state != NULL)
    return 1;

  mem = mmap(NULL, LUA_TLSF_POOL_SIZE, PROT_READ | PROT_WRITE,
             MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE, -1, 0);
  if (mem == MAP_FAILED)
    return 0;

  tlsf_state = tlsf_create_with_pool(mem, LUA_TLSF_POOL_SIZE);
  tlsf_mapping = mem;
  tlsf_mapping_size = LUA_TLSF_POOL_SIZE;
  atexit(tlsf_allocator_cleanup);
  return 1;
}


void *lua_tlsf_alloc(void *ud, void *ptr, size_t osize, size_t nsize) {
  (void)ud;
  (void)osize;

  if (nsize == 0) {
    if (tlsf_state != NULL)
      tlsf_free(tlsf_state, ptr);
    return NULL;
  }

  if (!tlsf_allocator_init())
    return NULL;

  return tlsf_realloc(tlsf_state, ptr, nsize);
}
