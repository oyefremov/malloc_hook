#pragma once

#include "malloc_hook.h"

#include <cstddef>
#include <limits>
#include <vector>

class test_allocator : public malloc_hook {
public:
  struct AllocationInfo {
    void *ptr;
    size_t size;
    size_t index;
  };

  void failAt(size_t index) noexcept;
  void abortAt(size_t index) noexcept;

  size_t maxTotallSize() const noexcept;
  size_t numAllocations() const noexcept;
  size_t currentAllocationsSizeInBytes() const noexcept;
  const std::vector<AllocationInfo> &leaks() const noexcept;
  const std::vector<void *> &unallocatedFree() const noexcept;

private:
  void *malloc(size_t size, malloc_func *defaultMalloc) noexcept override;
  void *realloc(void *ptr, size_t size,
                realloc_func *defaultRealloc) noexcept override;
  void *calloc(size_t num, size_t size,
               calloc_func *defaultCalloc) noexcept override;
  void free(void *ptr, free_func *systemFree) noexcept override;

  bool intercepAllocation(size_t size);
  void recordAllocation(void *ptr, size_t size);
  void freeAllocation(void *ptr);

  size_t m_totallSize = 0;
  size_t m_numAllocations = 0;
  size_t m_maxTotallSize = 0;
  std::vector<AllocationInfo> m_allocations;
  std::vector<void *> m_unallocatedFree;
  size_t m_failIndexe = std::numeric_limits<size_t>::max();
  size_t m_breakIndexe = std::numeric_limits<size_t>::max();
};
