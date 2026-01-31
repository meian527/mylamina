//
// Created by geguj on 2025/12/28.
//

#include "emit.hpp"
#include <bit>
#include <cstring>

namespace lmx {

void LMXOpcodeEmitter::write_imm(uint8_t *dst, int64_t imm) {
    memcpy(dst, &imm, sizeof(imm));
}

template<class... Args>
 void LMXOpcodeEmitter::write_regs(uint8_t *dst, Args... args) {
    ((*dst++ = std::forward<Args>(args)), ...);
}
void LMXOpcodeEmitter::emit_mov_ri(std::vector<lmx::runtime::Op> &ops, uint8_t r1, int64_t imm) {
    lmx::runtime::Op op(lmx::runtime::Opcode::MOV_RI);
    op.operands[0] = r1;
    memcpy(op.operands + 1, &imm, sizeof(imm));
    ops.push_back(op);
}
void LMXOpcodeEmitter::emit_mov_rr(std::vector<lmx::runtime::Op> &ops, uint8_t r1, uint8_t r2) {
    lmx::runtime::Op op(lmx::runtime::Opcode::MOV_RR);
    op.operands[0] = r1;
    op.operands[1] = r2;
    ops.push_back(op);
}
void LMXOpcodeEmitter::emit_div(std::vector<lmx::runtime::Op> &ops, uint8_t r1, uint8_t r2, uint8_t r3) {
    lmx::runtime::Op op(lmx::runtime::Opcode::DIV);
    write_regs(op.operands, r1, r2, r3);
    ops.push_back(op);
}
void LMXOpcodeEmitter::emit_add(std::vector<lmx::runtime::Op> &ops, uint8_t r1, uint8_t r2, uint8_t r3) {
    lmx::runtime::Op op(lmx::runtime::Opcode::ADD);
    write_regs(op.operands, r1, r2, r3);
    ops.push_back(op);
}
void LMXOpcodeEmitter::emit_sub(std::vector<lmx::runtime::Op> &ops, uint8_t r1, uint8_t r2, uint8_t r3) {
    lmx::runtime::Op op(lmx::runtime::Opcode::SUB);
    write_regs(op.operands, r1, r2, r3);
    ops.push_back(op);
}
void LMXOpcodeEmitter::emit_mul(std::vector<lmx::runtime::Op> &ops, uint8_t r1, uint8_t r2, uint8_t r3) {
    lmx::runtime::Op op(lmx::runtime::Opcode::MUL);
    write_regs(op.operands, r1, r2, r3);
    ops.push_back(op);
}
void LMXOpcodeEmitter::emit_mod(std::vector<lmx::runtime::Op> &ops, uint8_t r1, uint8_t r2, uint8_t r3) {
    lmx::runtime::Op op(lmx::runtime::Opcode::MOD);
    write_regs(op.operands, r1, r2, r3);
    ops.push_back(op);
}
void LMXOpcodeEmitter::emit_pow(std::vector<lmx::runtime::Op> &ops, uint8_t r1, uint8_t r2, uint8_t r3) {
    lmx::runtime::Op op(lmx::runtime::Opcode::POW);
    write_regs(op.operands, r1, r2, r3);
    ops.push_back(op);
}
void LMXOpcodeEmitter::emit_fcall(std::vector<lmx::runtime::Op> &ops, uint64_t idx) {
    lmx::runtime::Op op(lmx::runtime::Opcode::FCALL);
    write_imm(op.operands, std::bit_cast<int64_t>(idx));
    ops.push_back(op);
}
void LMXOpcodeEmitter::emit_halt(std::vector<lmx::runtime::Op> &ops) {
    lmx::runtime::Op op(lmx::runtime::Opcode::HALT);
    ops.push_back(op);
}
void LMXOpcodeEmitter::emit_fret(std::vector<lmx::runtime::Op> &ops) {
    lmx::runtime::Op op(lmx::runtime::Opcode::FRET);
    ops.push_back(op);
}
void LMXOpcodeEmitter::emit_debug_log(std::vector<lmx::runtime::Op> &ops, uint64_t idx) {
    lmx::runtime::Op op(lmx::runtime::Opcode::DEBUG_LOG);
    write_imm(op.operands, std::bit_cast<int64_t>(idx));
    ops.push_back(op);
}

void LMXOpcodeEmitter::emit_jmp(std::vector<lmx::runtime::Op> &ops, uint64_t idx) {
    lmx::runtime::Op op(lmx::runtime::Opcode::JMP);
    write_imm(op.operands, std::bit_cast<int64_t>(idx));
    ops.push_back(op);
}

void LMXOpcodeEmitter::emit_cmp_gt(std::vector<lmx::runtime::Op> &ops, uint8_t r1, uint8_t r2, uint8_t r3) {
    lmx::runtime::Op op(lmx::runtime::Opcode::CMP_GT);
    write_regs(op.operands, r1, r2, r3);
    ops.push_back(op);
}

void LMXOpcodeEmitter::emit_cmp_ge(std::vector<lmx::runtime::Op> &ops, uint8_t r1, uint8_t r2, uint8_t r3) {
    lmx::runtime::Op op(lmx::runtime::Opcode::CMP_GE);
    write_regs(op.operands, r1, r2, r3);
    ops.push_back(op);
}

void LMXOpcodeEmitter::emit_cmp_lt(std::vector<lmx::runtime::Op> &ops, uint8_t r1, uint8_t r2, uint8_t r3) {
    lmx::runtime::Op op(lmx::runtime::Opcode::CMP_LT);
    write_regs(op.operands, r1, r2, r3);
    ops.push_back(op);
}

void LMXOpcodeEmitter::emit_cmp_le(std::vector<lmx::runtime::Op> &ops, uint8_t r1, uint8_t r2, uint8_t r3) {
    lmx::runtime::Op op(lmx::runtime::Opcode::CMP_LE);
    write_regs(op.operands, r1, r2, r3);
    ops.push_back(op);
}

void LMXOpcodeEmitter::emit_cmp_eq(std::vector<lmx::runtime::Op> &ops, uint8_t r1, uint8_t r2, uint8_t r3) {
    lmx::runtime::Op op(lmx::runtime::Opcode::CMP_EQ);
    write_regs(op.operands, r1, r2, r3);
    ops.push_back(op);
}

void LMXOpcodeEmitter::emit_cmp_ne(std::vector<lmx::runtime::Op> &ops, uint8_t r1, uint8_t r2, uint8_t r3) {
    lmx::runtime::Op op(lmx::runtime::Opcode::CMP_NE);
    write_regs(op.operands, r1, r2, r3);
    ops.push_back(op);
}

void LMXOpcodeEmitter::emit_if_true(std::vector<lmx::runtime::Op> &ops, uint8_t r, uint64_t idx) {
    lmx::runtime::Op op(lmx::runtime::Opcode::IF_TRUE);
    op.operands[0] = r;
    write_imm(op.operands + 1, std::bit_cast<int64_t>(idx));
    ops.push_back(op);
}

void LMXOpcodeEmitter::emit_if_false(std::vector<lmx::runtime::Op> &ops, uint8_t r, uint64_t idx) {
    lmx::runtime::Op op(lmx::runtime::Opcode::IF_FALSE);
    op.operands[0] = r;
    write_imm(op.operands + 1, std::bit_cast<int64_t>(idx));
    ops.push_back(op);
}

// 新增指令的emit函数实现
void LMXOpcodeEmitter::emit_func_create(std::vector<lmx::runtime::Op> &ops, uint64_t idx) {
    lmx::runtime::Op op(lmx::runtime::Opcode::FUNC_CREATE);
    write_imm(op.operands, std::bit_cast<int64_t>(idx));
    ops.push_back(op);
}

void LMXOpcodeEmitter::emit_func_end(std::vector<lmx::runtime::Op> &ops) {
    lmx::runtime::Op op(lmx::runtime::Opcode::FUNC_END);
    ops.push_back(op);
}

void LMXOpcodeEmitter::emit_local_get_int(std::vector<lmx::runtime::Op> &ops, uint8_t r, uint8_t idx) {
    lmx::runtime::Op op(lmx::runtime::Opcode::LOCAL_GET_INT);
    op.operands[0] = r;
    op.operands[1] = idx;
    ops.push_back(op);
}

void LMXOpcodeEmitter::emit_local_set_int(std::vector<lmx::runtime::Op> &ops, uint8_t r, uint8_t idx) {
    lmx::runtime::Op op(lmx::runtime::Opcode::LOCAL_SET_INT);
    op.operands[0] = r;
    op.operands[1] = idx;
    ops.push_back(op);
}

void LMXOpcodeEmitter::emit_local_get_float(std::vector<lmx::runtime::Op> &ops, uint8_t r, uint8_t idx) {
    lmx::runtime::Op op(lmx::runtime::Opcode::LOCAL_GET_FLOAT);
    op.operands[0] = r;
    op.operands[1] = idx;
    ops.push_back(op);
}

void LMXOpcodeEmitter::emit_local_set_float(std::vector<lmx::runtime::Op> &ops, uint8_t r, uint8_t idx) {
    lmx::runtime::Op op(lmx::runtime::Opcode::LOCAL_SET_FLOAT);
    op.operands[0] = r;
    op.operands[1] = idx;
    ops.push_back(op);
}

void LMXOpcodeEmitter::emit_local_get_bool(std::vector<lmx::runtime::Op> &ops, uint8_t r, uint8_t idx) {
    lmx::runtime::Op op(lmx::runtime::Opcode::LOCAL_GET_BOOL);
    op.operands[0] = r;
    op.operands[1] = idx;
    ops.push_back(op);
}

void LMXOpcodeEmitter::emit_local_set_bool(std::vector<lmx::runtime::Op> &ops, uint8_t r, uint8_t idx) {
    lmx::runtime::Op op(lmx::runtime::Opcode::LOCAL_SET_BOOL);
    op.operands[0] = r;
    op.operands[1] = idx;
    ops.push_back(op);
}
} // namespace lmx




