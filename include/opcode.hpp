//
// Created by geguj on 2025/12/27.
//

#pragma once
#include <cstdint>
#include <cstring>

namespace lmx::runtime {
enum class OpPrefix {
    Integer, Float
};
enum class Opcode {
    /*
         * R = 寄存器 1
         * I = 立即数 8
         * M = 内存偏移 （1字节寄存器 + 1字节偏移）
         * C = 常量池偏移 8
         */
    MOV_RI, MOV_RM, MOV_RR, MOV_RC, //op dst(1), src
    MOV_MI, MOV_MM, MOV_MR, MOV_MC, //op dst(2), src
    ADD, SUB, MUL, DIV, MOD, POW,   //op dst(1), src1(1), src2(1)
    HALT,
    FCALL,  //op mem(8)
    FRET, DEBUG_LOG,
    JMP,
    CMP_GE, CMP_LT, CMP_LE, CMP_GT, CMP_EQ, CMP_NE,
    IF_TRUE,
    IF_FALSE,
    FUNC_CREATE, FUNC_END,
    LOCAL_GET, LOCAL_SET,

    AND, OR,
    VMC,  // vmcall
};

struct Op {
    Opcode op;
    uint8_t operands[12]{};

    explicit inline Op(const Opcode op, const uint8_t* operand): op(op) {
        memcpy(operands, operand, 12);
    }
    explicit inline Op(const Opcode op): op(op) {}
};
} // namespace lmx
