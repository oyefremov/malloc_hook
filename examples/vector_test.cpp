#include "test_allocator.h"
#include "gtest/gtest.h"
#include <vector>

TEST(VectorTest, BasicOperations) {
  test_allocator allocator;

  {
    std::vector<int> vec;

    for (int i = 0; i < 10; ++i) {
      vec.push_back(i);
    }

    EXPECT_EQ(vec.size(), 10);
    EXPECT_EQ(vec.capacity(), 16);

    vec.clear();
    EXPECT_EQ(vec.size(), 0);
    EXPECT_EQ(vec.capacity(), 16);

    EXPECT_EQ(allocator.leaks().size(), 1);
    EXPECT_EQ(allocator.currentAllocationsSizeInBytes(), 64);
  }
  allocator.disable();

  EXPECT_EQ(allocator.numAllocations(), 5);
  EXPECT_EQ(allocator.currentAllocationsSizeInBytes(), 0);
  EXPECT_EQ(allocator.leaks().size(), 0);
  EXPECT_EQ(allocator.unallocatedFree().size(), 0);
}