#include "test_allocator.h"

#include <algorithm>
#include <csignal>
#include <cstddef>
#include <vector>

void *
test_allocator::malloc(size_t size,
                       test_allocator::malloc_func *defaultMalloc) noexcept {
  if (m_breakIndexe == m_numAllocations) {
    // triggers a breakpoint
    raise(SIGTRAP);
  }
  if (m_failIndexe <= m_numAllocations) {
    ++m_numAllocations;
    return nullptr;
  }
  void *ptr = defaultMalloc(size);
  if (ptr) {
    recordAllocation(ptr, size);
  }
  return ptr;
}

void *test_allocator::realloc(void *ptr, size_t size,
                              realloc_func *defaultRealloc) noexcept {
  if (intercepAllocation(size)) {
    return nullptr;
  }
  void *newPtr = defaultRealloc(ptr, size);
  freeAllocation(ptr);
  recordAllocation(newPtr, size);
  return newPtr;
}

void *test_allocator::calloc(size_t num, size_t size,
                             calloc_func *defaultCalloc) noexcept {
  if (intercepAllocation(num * size)) {
    return nullptr;
  }
  void *ptr = defaultCalloc(num, size);
  recordAllocation(ptr, num * size);
  return ptr;
}

void test_allocator::free(void *ptr,
                          test_allocator::free_func *systemFree) noexcept {
  freeAllocation(ptr);
  systemFree(ptr);
}

bool test_allocator::intercepAllocation(size_t /*size*/) {
  if (m_breakIndexe == m_numAllocations) {
    // triggers a breakpoint
    raise(SIGTRAP);
  }
  if (m_failIndexe <= m_numAllocations) {
    ++m_numAllocations;
    return true;
  }
  return false;
}

void test_allocator::recordAllocation(void *ptr, size_t size) {
  AllocationInfo info;
  info.ptr = ptr;
  info.size = size;
  info.index = m_numAllocations;
  m_allocations.push_back(info);

  ++m_numAllocations;
  m_totallSize += size;
  if (m_totallSize > m_maxTotallSize) {
    m_maxTotallSize = m_totallSize;
  }
}

void test_allocator::freeAllocation(void *ptr) {
  if (ptr) {
    auto pos = std::find_if(
        m_allocations.begin(), m_allocations.end(),
        [ptr](const AllocationInfo &info) { return info.ptr == ptr; });
    if (pos == m_allocations.end()) {
      m_unallocatedFree.push_back(ptr);
    } else {
      m_totallSize -= pos->size;
      m_allocations.erase(pos);
    }
  }
}

void test_allocator::failAt(size_t index) noexcept { m_failIndexe = index; }

void test_allocator::abortAt(size_t index) noexcept { m_breakIndexe = index; }

size_t test_allocator::maxTotallSize() const noexcept {
  return m_maxTotallSize;
}
size_t test_allocator::currentAllocationsSizeInBytes() const noexcept {
  return m_totallSize;
}
size_t test_allocator::numAllocations() const noexcept {
  return m_numAllocations;
}
const std::vector<test_allocator::AllocationInfo> &
test_allocator::leaks() const noexcept {
  return m_allocations;
}
const std::vector<void *> &test_allocator::unallocatedFree() const noexcept {
  return m_unallocatedFree;
}
