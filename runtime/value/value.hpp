//
// Created by geguj on 2025/12/27.
//

#pragma once
#include <cstdint>
#include <string>
#include "../../include/lmx_export.hpp"

namespace lmx::runtime {

union LMVM_API Value {
    bool       b;
    int8_t     i8;
    uint8_t    u8;
    int16_t    i16;
    uint16_t   u16;
    int32_t    i32;
    uint32_t   u32;
    int64_t    i64;
    uint64_t   u64;
    float      f32;
    double     f64;
    std::nullptr_t null;
    void* ptr;
    char* str;

    explicit Value(void* p);
    Value();
    template<class T>
    T& get();

    Value &operator=(void* new_ptr);
};

}
