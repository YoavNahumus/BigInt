#pragma once

#ifdef __cplusplus
#include <cstddef>
#include <cstdint>
extern "C"
{
    bool big_int_add(uint64_t* dest, size_t size, const uint64_t* addend, size_t addend_size);
    bool big_int_sub(uint64_t* dest, size_t size, const uint64_t* addend, size_t addend_size);
}

#else
#include <stddef.h>
#include <stdint.h>
uint64_t big_int_add(uint64_t* dest, size_t size, const uint64_t* addend, size_t addend_size);
uint64_t big_int_sub(uint64_t* dest, size_t size, const uint64_t* addend, size_t addend_size);
#endif