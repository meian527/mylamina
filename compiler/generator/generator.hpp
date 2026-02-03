//
// Created by geguj on 2025/12/28.
//

#pragma once
#include <bitset>
#include <functional>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <vector>
#include <memory>
#include <ranges>

#include "../ast.hpp"

#include "../../include/lmx_export.hpp"
#include "../../include/opcode.hpp"

namespace lmx {

namespace runtime {
struct Op;
}
#define REG_COUNT 255
#define REG_COUNT_INDEX_MAX 254
class LMC_API Allocator {
    std::bitset<REG_COUNT> bitset;
public:
    Allocator();
    size_t alloc();
    size_t alloc(size_t i);
    void free(size_t i);
    bool is_free(size_t i);
};
class LMC_API Generator {

    /* ===========================
     * expr_release
     * type: bool
     * 标记expr_ret_reg是否可以结束后释放
    =========================== */
    bool expr_release{false};


    size_t gen_program(std::shared_ptr<ASTNode>& n);
    size_t gen_binary(std::shared_ptr<ASTNode>& n);



    size_t gen_function(std::shared_ptr<ASTNode>& n);
    size_t gen_num(std::shared_ptr<ASTNode>& n);
    size_t gen_assign(std::shared_ptr<ASTNode>& n);
    size_t gen_var_ref(std::shared_ptr<ASTNode>& n);

    size_t gen_vmc(std::shared_ptr<ASTNode> &n);

    size_t gen_return(std::shared_ptr<ASTNode>& n);
    size_t gen_func_call(std::shared_ptr<ASTNode>& n);

    size_t basic_gen_func_call(std::shared_ptr<ASTNode> &n, size_t args_idx);

    void basic_gen_pass_args(std::vector<std::shared_ptr<ASTNode>> &args, size_t args_idx, size_t args_count);

    size_t gen_unary(std::shared_ptr<ASTNode>& n);

    size_t gen_string(std::shared_ptr<ASTNode>& n);

    size_t gen_bool(std::shared_ptr<ASTNode>& n);

    size_t gen_block(std::shared_ptr<ASTNode>& n);

    auto tagging() const { return ops.size(); }
    void new_frame(const std::string& name) { cur = std::make_unique<CompilingFrame>(name, std::move(cur));}
    void free_frame() { cur = std::move(cur->last); }
    size_t gen_if(std::shared_ptr<ASTNode>& n);

    static void error(const std::string& msg) {
        node_has_error = true;
        std::cerr << "Error: " << msg << std::endl;
    }

    struct CompilingFrame {
        std::unique_ptr<CompilingFrame> last{nullptr};
        std::string name{"global"};
        std::unordered_map<std::string, std::pair<bool, uint16_t>> locals; // name <mutable, here>

        uint16_t local_count{static_cast<uint16_t>(-1)};

        explicit CompilingFrame(std::string name, std::unique_ptr<CompilingFrame>&& last): name(std::move(name)), last(std::move(last))  {}

        CompilingFrame() = default;

        /* ====================================== *
         * function find(name)
         * find the var named `name` up to frames
         *
         * @return
         * 一般情况： （frame*, (mutable, idx))
         * 未找到情况： （nullptr, (false, UINT16_MAX))
         * ======================================= */
        auto find(const std::string& nme) -> std::pair<CompilingFrame*, std::pair<bool, uint16_t>> {
            auto c = this;
            while (c) {
                // 在当前帧中查找
                for (const auto& [k, v] : c->locals)
                    if (k == nme)
                        return {c, v};
                // 跳转到上一帧
                if (c->last) {
                    c = c->last.get();
                } else {
                    c = nullptr;
                }
            }
            // 未找到
            return {nullptr, {false,UINT16_MAX}};
        }

        uint16_t new_var(const std::string& name, bool is_mut, uint16_t addr) {
            if (!locals.contains(name)) {
                if (addr > local_count) {
                    locals.reserve(addr + 1);
                    local_count = addr;
                }
                locals[name] = std::make_pair(is_mut, addr);
            } else error("redefined var: `" + name + "`");
            return local_count;
        }
        uint16_t new_var(const std::string& name, bool is_mut) {
            return new_var(name, is_mut, ++local_count);
        }
    };
    std::unique_ptr<CompilingFrame> cur;
    std::unordered_map<std::string, std::pair<uint8_t, size_t>> funcs;
    void new_func(const std::string & name, uint8_t size) {
        if (!funcs.contains(name)) {
            funcs[name] = std::make_pair(size, togging());
        } else error("redefined function: `" + name + "`");
    }
    void new_func(const std::string & name, uint8_t size, size_t addr) {
        if (funcs.find(name) == funcs.end()) {
            funcs[name] = std::make_pair(size, addr);
        } else error("redefined function: `" + name + "`");
    }
    bool find_func(const std::string & name) const {
        return funcs.contains(name);
    }
public:
    Allocator regs;
    Generator() : cur(std::make_unique<CompilingFrame>()){}
    ~Generator() = default;

    std::vector<runtime::Op> ops;
    void write(runtime::Op& op);


    std::vector<runtime::Op> get_ops();
    std::vector<char> constant_pool;


    size_t gen(std::shared_ptr<ASTNode> &n);

    [[nodiscard]] size_t togging() const {return ops.size();}

    void print_ops();

    void print_vars() {
        for (auto& v: cur->locals) {
            std::cout << v.first << " at address: " << v.second.second << " mutable: " << (v.second.first ? "true" : "false") << std::endl;
        }
    }
};

}
