#pragma once
#include <unordered_map>
#include <vector>

#include "base.hpp"
#include "../value/value.hpp"

namespace lmx::runtime {

class TableObj : BaseObj {
    std::vector<Value> front;
    std::unordered_map<Value, Value, ValueHasher> back;
public:
    TableObj() : BaseObj(LMXObjType::Table) {}

    TableObj(const TableObj& obj) noexcept = default;
    TableObj& operator=(const TableObj& obj) noexcept = default;
    TableObj& operator=(TableObj&& obj) noexcept = default;
    ~TableObj() = default;

    size_t size() const;

    Value &operator[](const Value &key);


    bool empty() const;

    void for_each(void(*fp)(Value& v));
};

}
