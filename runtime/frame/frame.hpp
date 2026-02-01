//
// Created by geguj on 2026/1/30.
//

#include <string>
#include <vector>
#include <memory>
#include "../value/value.hpp"


namespace lmx::runtime {
    struct StackFrame {
        std::unique_ptr<StackFrame> last{nullptr};
        // size_t local_pc{};
        std::vector<Value> locals;
        // size_t ret_addr{};

        StackFrame() = default;
        explicit StackFrame(std::unique_ptr<StackFrame>&& last) : last(std::move(last)) { locals.reserve(64); }

        void new_var(const uint16_t addr, const Value& value) {
            if (locals.size() <= addr) locals.resize(addr + 1);
            locals[addr] = value;
        }
    };
}

