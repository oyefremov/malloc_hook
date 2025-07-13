#pragma once
#include <cstddef>

class malloc_hook {
public:
    // Install hook
    malloc_hook();
    // Uninstall hook
    virtual ~malloc_hook();

    malloc_hook(const malloc_hook&) = delete;
    malloc_hook(malloc_hook&&) = delete;
    malloc_hook& operator=(const malloc_hook&) = delete;
    malloc_hook& operator=(malloc_hook&&) = delete;

    void enable();
    void disable();
    [[nodiscard]] bool isEnabled() const;

    using malloc_func = void*(size_t);
    using realloc_func = void*(void*, size_t);
    using calloc_func = void*(size_t, size_t);
    using free_func = void(void*);

    virtual void* malloc(size_t size, malloc_func* defaultMalloc) noexcept;
    virtual void* realloc(void* ptr, size_t size, realloc_func* defaultRealloc) noexcept;
    virtual void* calloc(size_t num, size_t size, calloc_func* defaultCalloc) noexcept;
    virtual void free(void* ptr, free_func* defaultFree) noexcept;
};
