//
// Created by geguj on 2025/12/28.
//

#include "generator.hpp"
#include <bitset>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "emit.hpp"
#include "../../include/opcode.hpp"

namespace lmx {

Allocator::Allocator() {
    bitset.reset();
    bitset.set(0);
}

size_t Allocator::alloc() {
    for (size_t i = 0; i < REG_COUNT; i++) {
        if (!bitset.test(i)) {
            bitset.set(i);
            return i;
        }
    }
    return -1;
}

size_t Allocator::alloc(size_t i) {
    bitset.set(i);
    return i;
}

void Allocator::free(size_t i) {
    if (i > 0 && bitset.test(i)) {
        bitset.reset(i);
    }
}

bool Allocator::is_free(size_t i) {
    return bitset.test(i);
}

void Generator::write(runtime::Op& op) {
    ops.push_back(op);
}


std::vector<lmx::runtime::Op> Generator::get_ops() {
    if (ops.back().op != runtime::Opcode::HALT) {
        ops.emplace_back(lmx::runtime::Opcode::HALT);
    }
    return ops;
}

size_t Generator::gen(std::shared_ptr<ASTNode> &n) {
    switch (n->kind) {
        case Program: return gen_program(n);
        case Binary: return gen_binary(n);
        case Unary: return gen_unary(n);
        case Return: return gen_return(n);
        case FuncCallExpr: return gen_func_call(n);
        case VarDecl: return gen_assign(n);
        case VarRef: return gen_var_ref(n);
        case NumLiteral: return gen_num(n);
        case StringLiteral: return gen_string(n);
        case BoolLiteral: return gen_bool(n);
        case BlockStmt: return gen_block(n);
        case IfStmt: return gen_if(n);
        case FuncDecl: return gen_function(n);
        default: return 0ULL;
    }
}

size_t Generator::gen_program(std::shared_ptr<ASTNode> &n) {
    const auto node = std::static_pointer_cast<ProgramASTNode>(std::move(n));
    size_t last_ret = 0;
    for (auto& c: node->children) {
        last_ret = gen(c);
    }
    return last_ret;
}

size_t Generator::gen_binary(std::shared_ptr<ASTNode>& n) {
    const auto node = std::static_pointer_cast<BinaryNode>(std::move(n));
    volatile size_t tmp = regs.alloc();
    volatile auto lr = gen(node->left);
    LMXOpcodeEmitter::emit_mov_rr(ops, tmp, lr);
    if (expr_release) regs.free(lr);
    lr = tmp;

    volatile size_t expr_ret_reg;
    if (node->op == "|>") {
        LMXOpcodeEmitter::emit_mov_rr(ops, REG_COUNT_INDEX_MAX, lr);
        expr_ret_reg = basic_gen_func_call(node->right, 1);
        expr_release = true;
        return expr_ret_reg;
    }
    volatile size_t rr = gen(node->right);
    tmp = regs.alloc();
    LMXOpcodeEmitter::emit_mov_rr(ops, tmp, rr);
    if (expr_release) regs.free(rr);
    rr = tmp;
    expr_ret_reg = regs.alloc();

    if (node->op == "!=") {
        LMXOpcodeEmitter::emit_cmp_ne(ops, expr_ret_reg, lr, rr);
    } else if (node->op == "<=") {
        LMXOpcodeEmitter::emit_cmp_le(ops, expr_ret_reg, lr, rr);
    } else if (node->op == ">=") {
        LMXOpcodeEmitter::emit_cmp_ge(ops, expr_ret_reg, lr, rr);
    } else if (node->op == "==") {
        LMXOpcodeEmitter::emit_cmp_eq(ops, expr_ret_reg, lr, rr);
    } else if (node->op == "<")  {
        LMXOpcodeEmitter::emit_cmp_lt(ops, expr_ret_reg, lr, rr);
    } else if (node->op == ">")  {
        LMXOpcodeEmitter::emit_cmp_gt(ops, expr_ret_reg, lr, rr);
    } else if (node->op == "+")  {
        LMXOpcodeEmitter::emit_add(ops, expr_ret_reg, lr, rr);
    } else if (node->op == "-")  {
        LMXOpcodeEmitter::emit_sub(ops, expr_ret_reg, lr, rr);
    } else if (node->op == "*")  {
        LMXOpcodeEmitter::emit_mul(ops, expr_ret_reg, lr, rr);
    } else if (node->op == "/")  {
        LMXOpcodeEmitter::emit_div(ops, expr_ret_reg, lr, rr);
    } else if (node->op == "%")  {
        LMXOpcodeEmitter::emit_mod(ops, expr_ret_reg, lr, rr);
    } else if (node->op == "^")  {
        LMXOpcodeEmitter::emit_pow(ops, expr_ret_reg, lr, rr);
    } else if (node->op == "||") {
        LMXOpcodeEmitter::emit_or(ops, expr_ret_reg, lr, rr);
    } else if (node->op == "&&") {
        LMXOpcodeEmitter::emit_and(ops, expr_ret_reg, lr, rr);
    }
    else {
        node_has_error = true;
        error("operator: `" + node->op + "' is not defined.");
        return -1;
    }
    expr_release = true;
    return expr_ret_reg;
}

size_t Generator::gen_num(std::shared_ptr<ASTNode>& n) {
    const auto node = std::static_pointer_cast<NumberNode>(std::move(n));
    const auto expr_ret_reg = regs.alloc();
    expr_release = true;
    LMXOpcodeEmitter::emit_mov_ri(ops, expr_ret_reg, std::stoll(node->num));
    return expr_ret_reg;
}
size_t Generator::gen_assign(std::shared_ptr<ASTNode>& n) {
    const auto node = std::static_pointer_cast<VarDeclNode>(std::move(n));
    size_t expr_ret_reg = 0;
    if (const auto [fst, snd] = cur->find(node->name); fst) {
        //如果可变
        if (snd.first) {
            expr_ret_reg = gen(node->value);
            LMXOpcodeEmitter::emit_local_set_int(ops, fst->locals[node->name].second, expr_ret_reg);
        } else {
            error("The var `" + node->name + "' is not mutable.");
        }
    } else { //如果未定义
        expr_ret_reg = gen(node->value);
        LMXOpcodeEmitter::emit_local_set_int(ops, cur->new_var(node->name, node->is_mut), expr_ret_reg);
    }
    regs.free(expr_ret_reg);
    return -1;
}
size_t Generator::gen_var_ref(std::shared_ptr<ASTNode>& n) {
    const auto node = std::static_pointer_cast<VarRefNode>(std::move(n));
    const auto [c, t] = cur->find(node->name);
    size_t expr_ret_reg = regs.alloc();
    if (t.second != UINT16_MAX) {
        LMXOpcodeEmitter::emit_local_get_int(ops, expr_ret_reg, t.second);
    } else {
        regs.free(expr_ret_reg);
        expr_ret_reg = -1;
        std::cerr << "The var `" + node->name + "` is not found!" << std::endl;
    }
    expr_release = false;
    return expr_ret_reg;
}

size_t Generator::gen_return(std::shared_ptr<ASTNode> &n) {
    const auto node = std::static_pointer_cast<ReturnStmtNode>(std::move(n));
    const auto expr_ret_reg = gen(node->expr);
    LMXOpcodeEmitter::emit_mov_rr(ops, 0, expr_ret_reg);
    LMXOpcodeEmitter::emit_fret(ops);
    if (expr_release) regs.free(expr_ret_reg);
    return 0;
}

size_t Generator::gen_function(std::shared_ptr<ASTNode> &n) {
    const auto node = std::static_pointer_cast<FuncDeclNode>(std::move(n));
    LMXOpcodeEmitter::emit_func_create(ops);
    new_frame(node->name);
    const auto args_count = node->args.size();

    new_func(node->name, args_count); //函数不做作用域区分，全部全局
    for (size_t i = 0; i < args_count ; i++) {
        LMXOpcodeEmitter::emit_local_set_int(ops, i, REG_COUNT_INDEX_MAX - i);
        cur->new_var(node->args[i], true, i);
    }
    if (node->body->kind != BlockStmt) error("expected block statement.");
    gen(node->body);
    LMXOpcodeEmitter::emit_fret(ops);
    LMXOpcodeEmitter::emit_func_end(ops);
    free_frame();
    return -1;
}

size_t Generator::gen_func_call(std::shared_ptr<ASTNode> &n) {
    return basic_gen_func_call(n, 0);
}
size_t Generator::basic_gen_func_call(std::shared_ptr<ASTNode> &n, const size_t args_idx) {
    const auto node = std::static_pointer_cast<FuncCallExprNode>(std::move(n));
    if (find_func(node->name)) {
        const auto& [tar_ac, addr] = funcs[node->name];
        const auto args_count = node->args.size();
        if (args_count + args_idx != tar_ac) {
            error(
        "args num mismatch: "
            + std::to_string(args_count)
            + " != "
            + std::to_string(tar_ac) + " at function calling `" + node->name + "`");
            return -1;
        }
        for (size_t i = args_idx; i < args_count ; i++) {
            const auto expr_ret = gen(node->args[i]);

            LMXOpcodeEmitter::emit_mov_rr(ops , REG_COUNT_INDEX_MAX - i, expr_ret);
            if (expr_release) regs.free(expr_ret);
        }
        LMXOpcodeEmitter::emit_fcall(ops, addr, tar_ac);
    } else error("The function `" + node->name + "` is not defined.");
    return 0;
}

size_t Generator::gen_unary(std::shared_ptr<ASTNode> &n) {
    const auto node = std::static_pointer_cast<UnaryNode>(std::move(n));
    const auto expr_ret_reg = gen(node->operand);


    const auto tmp_reg = regs.alloc();
    LMXOpcodeEmitter::emit_mov_ri(ops, tmp_reg, 0);
    regs.free(tmp_reg);

    const auto result = regs.alloc();

    if (node->op == "!") {
        LMXOpcodeEmitter::emit_cmp_eq(ops, result, expr_ret_reg, tmp_reg);
    } else if (node->op == "-") {
        LMXOpcodeEmitter::emit_sub(ops, result, tmp_reg, expr_ret_reg);
    } else {
        error("unary operator: `" + node->op + "' is not defined.");
    }
    if (expr_release) regs.free(expr_ret_reg);
    expr_release = true;
    return result;
}

size_t Generator::gen_string(std::shared_ptr<ASTNode> &n) {
    const auto node = std::static_pointer_cast<StringNode>(std::move(n));
    const auto expr_ret = constant_pool.size();
    constant_pool.insert(constant_pool.end(), node->str.begin(), node->str.end());
    expr_release = false;
    return expr_ret;
}

size_t Generator::gen_bool(std::shared_ptr<ASTNode> &n) {
    const auto node = std::static_pointer_cast<BoolNode>(std::move(n));
    const auto expr_ret_reg = regs.alloc();
    LMXOpcodeEmitter::emit_mov_ri(ops, expr_ret_reg, node->b);
    expr_release = true;
    return expr_ret_reg;
}

size_t Generator::gen_block(std::shared_ptr<ASTNode> &n) {
    const auto node = std::static_pointer_cast<BlockStmtNode>(std::move(n));
    size_t expr_ret_reg = 0;
    for (auto& c: node->children) expr_ret_reg = gen(c);
    return expr_ret_reg;
}

size_t Generator::gen_if(std::shared_ptr<ASTNode> &n) {
    const auto node = std::static_pointer_cast<IfStmtNode>(std::move(n));
    const auto cond = gen(node->condition);

    const auto the = tagging();
    LMXOpcodeEmitter::emit_if_true(ops, cond, 0);

    const auto els = tagging();
    LMXOpcodeEmitter::emit_jmp(ops, 0);

    auto tmp = tagging();
    memcpy(ops[the].operands + 1, &tmp, sizeof(tmp));
    static size_t counter = 0;
    new_frame("@if@_" + std::to_string(counter++));
    gen(node->thenBlock);
    free_frame();
    tmp = tagging();
    memcpy(ops[els].operands, &tmp, sizeof(tmp));
    if (node->elseBlock) {
        new_frame("@else@_" + std::to_string(counter++));
        gen(node->elseBlock);
        free_frame();
    }
    return -1;
}

void Generator::print_ops() {
    size_t i = 0;
    for (auto &op: ops) {
        printf("[0x%llx]\t", i++);
        switch (op.op) {
            using enum runtime::Opcode;
        case MOV_RI: {
            printf("MOVRI: %d, %lld\n", op.operands[0], *reinterpret_cast<int64_t *>(op.operands + 1));
            break;
        }
        case MOV_RM: {
            printf("MOVRM: %d, 0x%llu\n", op.operands[0], *reinterpret_cast<uint64_t *>(op.operands + 1));
            break;
        }
        case MOV_RR: {
            printf("MOVRR: %u, %u\n", op.operands[0], op.operands[1]);
            break;
        }
        case MOV_RC: {
            printf("MOVRC: %u, 0x%llu\n", op.operands[0], *reinterpret_cast<uint64_t *>(op.operands + 1));
            break;
        }
        case MOV_MI: {

        }
        case MOV_MM: {
        }
        case MOV_MR: {
        }
        case MOV_MC: {
        }
        case ADD: {
            printf("ADD: %u, %u, %u\n", op.operands[0], op.operands[1], op.operands[2]);
            break;
        }
        case SUB: {
            printf("SUB: %u, %u, %u\n", op.operands[0], op.operands[1], op.operands[2]);
            break;
        }
        case MUL: {
            printf("MUL: %u, %u, %u\n", op.operands[0], op.operands[1], op.operands[2]);
            break;
        }
        case DIV: {
            printf("DIV: %u, %u, %u\n", op.operands[0], op.operands[1], op.operands[2]);
            break;
        }
        case MOD: {
            printf("MOD: %u, %u, %u\n", op.operands[0], op.operands[1], op.operands[2]);
            break;
        }
        case POW: {
            printf("POW: %u, %u, %u\n", op.operands[0], op.operands[1], op.operands[2]);
            break;
        }
        case FCALL: {
            printf("FCALL: 0x%llx, 0x%u\n", *reinterpret_cast<uint64_t *>(op.operands), op.operands[8]);
            break;
        }
        case FRET: {
            printf("FRET\n");
            break;
        }
        case HALT: {
            printf("HALT\n");
            break;
        }
        case DEBUG_LOG: {
            printf("DEBUG_LOG\n");
            break;
        }
        case JMP: {
            printf("JMP: 0x%llx\n", *reinterpret_cast<uint64_t *>(op.operands));
            break;
        }
        case CMP_GE: {
            printf("CMP_GE: %u, %u, %u\n", op.operands[0], op.operands[1], op.operands[2]);
            break;
        }
        case CMP_LT: {
            printf("CMP_LT: %u, %u, %u\n", op.operands[0], op.operands[1], op.operands[2]);
            break;
        }
        case CMP_LE: {
            printf("CMP_LE: %u, %u, %u\n", op.operands[0], op.operands[1], op.operands[2]);
            break;
        }
        case CMP_GT: {
            printf("CMP_GT: %u, %u, %u\n", op.operands[0], op.operands[1], op.operands[2]);
            break;
        }
        case CMP_EQ: {
            printf("CMP_EQ: %u, %u, %u\n", op.operands[0], op.operands[1], op.operands[2]);
            break;
        }
        case CMP_NE: {
            printf("CMP_NE: %u, %u, %u\n", op.operands[0], op.operands[1], op.operands[2]);
            break;
        }
        case IF_TRUE: {
            printf("IF_TRUE: %u, 0x%llx\n", op.operands[0], *reinterpret_cast<uint64_t *>(op.operands + 1));
            break;
        }
        case IF_FALSE: {
            printf("IF_FALSE: %u, 0x%llx\n", op.operands[0], *reinterpret_cast<uint64_t *>(op.operands + 1));
            break;
        }
        case LOCAL_GET_BOOL: {
            printf("LOCAL_GET_BOOL: %u, 0x%x\n", op.operands[0], *(uint16_t*)(op.operands + 1));
            break;
        }
        case LOCAL_GET_INT: {
            printf("LOCAL_GET_INT: %u, 0x%x\n", op.operands[0], *(uint16_t*)(op.operands + 1));
            break;
        }
        case LOCAL_GET_FLOAT: {
            printf("LOCAL_GET_FLOAT: %u, 0x%x\n", op.operands[0], *(uint16_t*)(op.operands + 1));
            break;
        }
        case LOCAL_SET_BOOL: {
            printf("LOCAL_SET_BOOL: 0x%x, %u\n", *(uint16_t*)op.operands, op.operands[2]);
            break;
        }
        case LOCAL_SET_INT: {
            printf("LOCAL_SET_INT: 0x%x, %u\n", *(uint16_t*)op.operands, op.operands[2]);
            break;
        }
        case LOCAL_SET_FLOAT: {
            printf("LOCAL_SET_FLOAT: 0x%x, %u\n", *(uint16_t*)op.operands, op.operands[2]);
            break;
        }
        case FUNC_CREATE: {
            printf("FUNC_CREATE\n");
            break;
        }
        case FUNC_END: {
            printf("FUNC_END\n");
            break;
        }
        case AND: {
            printf("AND: %u, %u\n", op.operands[0], op.operands[1]);
            break;
        }
        case OR: {
            printf("OR: %u, %u\n", op.operands[0], op.operands[1]);
            break;
        }
        case VMC: {
            printf("VMC: %d\n", *(uint16_t*)op.operands);
            break;
        }
        }
    }

    printf("[0x%llx]\tHALT\n", i);
}



} // namespace lmx
