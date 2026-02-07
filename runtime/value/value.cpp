//
// Created by geguj on 2025/12/27.
//

#include "value.hpp"

namespace lmx::runtime {

Value::Value() : null(nullptr), type(ValueType::Ptr) {
}

Value::Value(void* p) : i64(*static_cast<int64_t*>(p)), type(ValueType::Ptr) {
}

Value& Value::operator=(void* new_ptr) {
    this->ptr = new_ptr;
    this->type = ValueType::Ptr;
    return *this;
}
Value& Value::operator=(bool b) {
    this->b = b;
    this->type = ValueType::Bool;
    return *this;
}
Value& Value::operator=(char* n) {
    this->str = n;
    this->type = ValueType::Str;
    return *this;
}
Value& Value::operator=(int64_t n) {
    this->i64 = n;
    this->type = ValueType::Int;
    return *this;
}
Value& Value::operator=(double n) {
    this->f64 = n;
    this->type = ValueType::Float;
    return *this;
}
Value& Value::operator=(uint8_t n) {
    this->u8 = n;
    this->type = ValueType::Int;
    return *this;
}
Value& Value::operator=(uint64_t n) {
    this->u64 = n;
    this->type = ValueType::Int;
    return *this;
}
Value& Value::operator=(const Value& rhs) {
    this->type = rhs.type;
    switch (this->type) {
        case ValueType::Int: this->i64 = rhs.i64; break;
        case ValueType::Float: this->f64 = rhs.f64; break;
        case ValueType::Str: this->str = rhs.str; break;
        case ValueType::Bool: this->b = rhs.b; break;
        case ValueType::Ptr: this->ptr = rhs.ptr; break;

    }
    return *this;
}


template<class T>
T &Value::get() {
    return *reinterpret_cast<T*>(this);
}

const char* Value::type_name() const {
    return ValueType::type_name(this->type);
}

std::string Value::to_string() const {
    switch (this->type) {
        case ValueType::Int:    return std::to_string(this->i64);
        case ValueType::Bool:   return this->b ? "true" : "false";
        case ValueType::Str:    return this->str;
        case ValueType::Float:  return std::to_string(this->f64);
        default: return {};
    }
}
} // namespace lmx::runtime