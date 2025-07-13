#include "fuzzy_allocator.h"

#include <csignal>
#include <cstddef>

void *
fuzzy_allocator::malloc(size_t size,
                        fuzzy_allocator::malloc_func *defaultMalloc) noexcept {
  if (interceptAllocation(size)) {
    return nullptr;
  }
  return defaultMalloc(size);
}

void *fuzzy_allocator::realloc(void *ptr, size_t size,
                               realloc_func *defaultRealloc) noexcept {
  if (interceptAllocation(size)) {
    return nullptr;
  }
  return defaultRealloc(ptr, size);
}

void *fuzzy_allocator::calloc(size_t num, size_t size,
                              calloc_func *defaultCalloc) noexcept {
  if (interceptAllocation(num * size)) {
    return nullptr;
  }
  return defaultCalloc(num, size);
}

bool fuzzy_allocator::interceptAllocation(size_t size) {
  if (m_breakIndex == m_numAllocations) {
    // triggers a breakpoint
    raise(SIGTRAP);
  }
  if (m_failIndex <= m_numAllocations) {
    ++m_numAllocations;
    return true;
  }
  if (size > m_maxAllocationSize) {
    m_maxAllocationSize = size;
    m_maxAllocationIndex = m_numAllocations;
  }
  ++m_numAllocations;
  return false;
}

void fuzzy_allocator::failAfter(size_t index) noexcept { m_failIndex = index; }

void fuzzy_allocator::abortAt(size_t index) noexcept { m_breakIndex = index; }

size_t fuzzy_allocator::numAllocations() const noexcept {
  return m_numAllocations;
}

size_t fuzzy_allocator::maxAllocationSize() const noexcept {
  return m_maxAllocationSize;
}

size_t fuzzy_allocator::maxAllocationIndex() const noexcept {
  return m_maxAllocationIndex;
}
