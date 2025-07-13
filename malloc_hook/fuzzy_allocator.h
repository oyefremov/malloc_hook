#pragma once

#include "malloc_hook.h"

#include <cstddef>
#include <limits>

class fuzzy_allocator : public malloc_hook {
public:
  size_t numAllocations() const noexcept;
  size_t maxAllocationSize() const noexcept;
  size_t maxAllocationIndex() const noexcept;

  void failAfter(size_t index) noexcept;
  void abortAt(size_t index) noexcept;

private:
  void *malloc(size_t size, malloc_func *defaultMalloc) noexcept override;
  void *realloc(void *ptr, size_t size,
                realloc_func *defaultRealloc) noexcept override;
  void *calloc(size_t num, size_t size,
               calloc_func *defaultCalloc) noexcept override;

  bool interceptAllocation(size_t size);

  size_t m_numAllocations = 0;
  size_t m_failIndex = std::numeric_limits<size_t>::max();
  size_t m_breakIndex = std::numeric_limits<size_t>::max();
  size_t m_maxAllocationSize = 0;
  size_t m_maxAllocationIndex = 0;
};
