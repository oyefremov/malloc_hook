add_library(malloc_hook STATIC)

target_sources(malloc_hook PRIVATE fuzzy_allocator.cpp malloc_hook.cpp
                                   test_allocator.cpp)

target_include_directories(malloc_hook PUBLIC .)
