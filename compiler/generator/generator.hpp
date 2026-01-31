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
     * 标记上一个gen的返回寄存器是否可以结束后释放
    =========================== */
    bool expr_release{false};


    size_t gen_program(std::shared_ptr<ASTNode>& n);
    size_t gen_binary(std::shared_ptr<ASTNode>& n);
    size_t gen_function(std::shared_ptr<ASTNode>& n);
    size_t gen_num(std::shared_ptr<ASTNode>& n);
    size_t gen_assign(std::shared_ptr<ASTNode>& n);
    size_t gen_var_ref(std::shared_ptr<ASTNode>& n);
    size_t gen_return(std::shared_ptr<ASTNode>& n);
    size_t gen_func_call(std::shared_ptr<ASTNode>& n);

    size_t gen_unary(std::shared_ptr<ASTNode>& n);

    size_t gen_string(std::shared_ptr<ASTNode>& n);

    size_t gen_bool(std::shared_ptr<ASTNode>& n);

    size_t gen_block(std::shared_ptr<ASTNode>& n);

    size_t gen_if(std::shared_ptr<ASTNode>& n);

    static void error(const std::string& msg) {
        node_has_error = true;
        std::cerr << "Error: " << msg << std::endl;
    }

    struct CompilingFrame {
        std::unique_ptr<CompilingFrame> last{nullptr};
        std::string name{"global"};
        std::unordered_map<std::string, std::pair<bool, size_t>> locals; // name <mutable, here(locals index)>

        explicit CompilingFrame(std::string name, const size_t ret_addr): name(std::move(name)) {}

        CompilingFrame() = default;

        /* ====================================== *
         * function find(name)
         * find the var named `name` up to frames
         *
         * @return
         * 一般情况： （frame*, idx)
         * 未找到情况： （frame*, SIZE_MAX) and cerr
         * ======================================= */
        auto find(const std::string& nme) {
            using Tp_ = std::pair<CompilingFrame*, std::pair<bool,size_t>>;
            std::function<
                Tp_(CompilingFrame*, const std::string&)>
            finder = [&finder](CompilingFrame* cur, const std::string& name) {
                for (const auto& [k, v]: cur->locals)
                    if (k == name) return std::make_pair(cur, v);
                if (cur->last) return finder(cur->last.get(), name);

                return std::make_pair(cur, std::make_pair(false, SIZE_MAX));
            };
            return finder(this, nme);
        }
    };
    std::unique_ptr<CompilingFrame> cur;
public:
    Allocator regs;
    Generator() : cur(std::make_unique<CompilingFrame>()){}
    ~Generator() = default;

    std::vector<runtime::Op> ops;
    void write(runtime::Op& op);


    std::vector<runtime::Op> get_ops();
    std::vector<char> constant_pool;


    size_t gen(std::shared_ptr<ASTNode>& n);

    void print_ops();

    [[nodiscard]] size_t tagging() const { return ops.size(); }
};

}
