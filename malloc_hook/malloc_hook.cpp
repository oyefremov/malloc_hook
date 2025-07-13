#include "malloc_hook.h"

#include <dlfcn.h>

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <utility>

static malloc_hook *g_mallocHook = nullptr;

malloc_hook::malloc_hook() { enable(); }

malloc_hook::~malloc_hook() {
  if (g_mallocHook) {
    disable();
  }
}

void *malloc_hook::malloc(size_t size, malloc_func *defaultMalloc) noexcept {
  return defaultMalloc(size);
}

void *malloc_hook::realloc(void *ptr, size_t size,
                           realloc_func *defaultRealloc) noexcept {
  return defaultRealloc(ptr, size);
}

void *malloc_hook::calloc(size_t num, size_t size,
                          calloc_func *defaultCalloc) noexcept {
  return defaultCalloc(num, size);
}

void malloc_hook::free(void *ptr, free_func *defaultFree) noexcept {
  defaultFree(ptr);
}

bool malloc_hook::isEnabled() const { return g_mallocHook == this; }

void malloc_hook::enable() {
  assert(g_mallocHook == nullptr);
  g_mallocHook = this;
}

void malloc_hook::disable() {
  assert(g_mallocHook == this);
  g_mallocHook = nullptr;
}

extern "C" void *malloc(size_t size) {
  static auto *defaultMalloc =
      reinterpret_cast<decltype(&malloc)>(dlsym(RTLD_NEXT, "malloc"));
  assert(defaultMalloc);

  void *ptr = nullptr;
  if (g_mallocHook) {
    auto *hook = std::exchange(g_mallocHook, nullptr);
    ptr = hook->malloc(size, defaultMalloc);
    g_mallocHook = hook;
  } else {
    ptr = defaultMalloc(size);
  }
  return ptr;
}

extern "C" void *realloc(void *ptr, size_t newSize) {
  static auto *defaultRealloc =
      reinterpret_cast<decltype(&realloc)>(dlsym(RTLD_NEXT, "realloc"));
  assert(defaultRealloc);

  if (g_mallocHook) {
    auto *hook = std::exchange(g_mallocHook, nullptr);
    ptr = hook->realloc(ptr, newSize, defaultRealloc);
    g_mallocHook = hook;
  } else {
    ptr = defaultRealloc(ptr, newSize);
  }
  return ptr;
}

extern "C" void *calloc(size_t num, size_t size) {
  static auto *defaultCalloc =
      reinterpret_cast<decltype(&calloc)>(dlsym(RTLD_NEXT, "calloc"));
  assert(defaultCalloc);

  void *ptr = nullptr;
  if (g_mallocHook) {
    auto *hook = std::exchange(g_mallocHook, nullptr);
    ptr = hook->calloc(num, size, defaultCalloc);
    g_mallocHook = hook;
  } else {
    ptr = defaultCalloc(num, size);
  }
  return ptr;
}

extern "C" void free(void *ptr) {
  static auto *defaultFree =
      reinterpret_cast<decltype(&free)>(dlsym(RTLD_NEXT, "free"));
  assert(defaultFree);

  if (g_mallocHook) {
    auto *hook = std::exchange(g_mallocHook, nullptr);
    hook->free(ptr, defaultFree);
    g_mallocHook = hook;
  } else {
    defaultFree(ptr);
  }
}
