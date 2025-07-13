add_executable(vector_test vector_test.cpp)

target_link_libraries(vector_test PRIVATE malloc_hook gtest_main)
