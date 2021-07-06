//
// Created by mzi12 on 2021/7/1.
//

#ifndef RISCSIMULATOR_OPERATOR_H

#include <iostream>
#include <cstring>

namespace Mzu {
    enum opType {
        LUI, AUIPC, JAL, JALR, BEQ, BNE, BLT, BGE, BLTU, BGEU, LB, LH, LW, LBU, LHU, SB, SH, SW,
        ADDI, SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAI, ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND, NOPE
    };

    class RegisterJar {
        static const uint reg_num = 32;
        uint reg[reg_num];
        bool reg_busy[reg_num];

    public:
        RegisterJar() {
            memset(reg, 0, sizeof(reg));
            memset(reg_busy, 0, sizeof(reg_busy));

        }

        const uint &operator[](const uint &i) const {
            return reg[i];
        }

        uint &operator[](const uint &i) {
            if (i == 0) return reg[0] = 0;
            return reg[i];
        }

        const bool &check(const uint &i) const {
            return reg_busy[i];
        }

        bool &check(const uint &i) {
            return reg_busy[i];
        }
    };
}
#define RISCSIMULATOR_OPERATOR_H

#endif //RISCSIMULATOR_OPERATOR_H
