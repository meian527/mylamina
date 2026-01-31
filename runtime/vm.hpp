//
// Created by geguj on 2025/12/27.
//
#pragma once
#include <array>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>
#include "../include/lmx_export.hpp"
#include "value/value.hpp"
#include "../include/opcode.hpp"
#include "frame/frame.hpp"

namespace lmx::runtime {

struct LMVM_API LMXState {
    size_t pc{0};
    std::array<Value, 255> regs{};

    std::vector<size_t> ret_addr_stack;
    //void* const_pool_top;
    std::vector<Op>* program;

    std::unique_ptr<StackFrame> cur;
};
class LMVM_API VirtualCore {
    void* const_pool_top;
    LMXState ste;

    [[nodiscard]] Value *get_value_from_pool(const size_t offest) const;
public:
    VirtualCore();
    VirtualCore(const VirtualCore&) = delete;
    VirtualCore& operator=(const VirtualCore&) = delete;
    VirtualCore(VirtualCore&&) = delete;
    explicit VirtualCore(LMXState ste);
    explicit VirtualCore(LMXState ste, void* const_pool_top);
    int run();

    [[nodiscard]] std::vector<Op> *get_program() const { return ste.program; }
    void set_program(std::vector<Op> *program) { ste.pc = 0;ste.program = program; }
    [[nodiscard]] int64_t look_register(const size_t r) const { return ste.regs[r].i64; }
};

}
