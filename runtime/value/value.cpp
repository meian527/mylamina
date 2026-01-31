//
// Created by geguj on 2025/12/27.
//

#include "value.hpp"

namespace lmx::runtime {

Value::Value() : null(nullptr) {
}

Value::Value(void* p) : i64(*static_cast<int64_t*>(p)) {
}

Value &Value::operator=(void* new_ptr) {
    this->ptr = new_ptr;
    return *this;
}

template<class T>
T &Value::get() {
    return *reinterpret_cast<T*>(this);
}

} // namespace lmx::runtime