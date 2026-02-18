//
// Created by geguj on 2026/2/12.
//

#include "table.hpp"

#include <ranges>

namespace lmx::runtime {

bool TableObj::empty() const {
    return front.empty() && back.empty();
}
size_t TableObj::size() const {
    return front.size() + back.size();
}
Value& TableObj::operator[](const Value& key) {
    if (key.type == ValueType::Int) return front.at(key.u64);
    return back.at(key);
}

void TableObj::for_each(void(*fp)(Value& v)) {
    for (auto& v : front) fp(v);
    for (auto& v : back | std::views::values) fp(v);
}


}
