//
// Created by mzi12 on 2021/7/1.
//

#ifndef RISCSIMULATOR_PARSER_H

#include "Operator.h"
#include "MemoryBox.h"

using namespace Mzu;

namespace Mzu {
#define END 0x0ff00513u

    opType getType(uint op) {
        uint opcode = op & 0b1111111u;
        uint funct3 = (op >> 12) & 0b111u;
        uint funct7 = (op >> 25) & 0b1111111u;
        switch (opcode) {
            case 0b0110111u:
                return LUI;
            case 0b0010111u:
                return AUIPC;
            case 0b1101111u:
                return JAL;
            case 0b1100111u:
                return JALR;
            case 0b1100011u: {
                switch (funct3) {
                    case 0b000u:
                        return BEQ;
                    case 0b001u:
                        return BNE;
                    case 0b100u:
                        return BLT;
                    case 0b101u:
                        return BGE;
                    case 0b110u:
                        return BLTU;
                    case 0b111u:
                        return BGEU;
                }
            }
            case 0b0000011u: {
                switch (funct3) {
                    case 0b000u:
                        return LB;
                    case 0b001u:
                        return LH;
                    case 0b010u:
                        return LW;
                    case 0b100u:
                        return LBU;
                    case 0b101u:
                        return LHU;
                }
            }
            case 0b0100011u: {
                switch (funct3) {
                    case 0b000u:
                        return SB;
                    case 0b001u:
                        return SH;
                    case 0b010u:
                        return SW;
                }
            }
            case 0b0010011u: {
                switch (funct3) {
                    case 0b000u:
                        return ADDI;
                    case 0b010u:
                        return SLTI;
                    case 0b011u:
                        return SLTIU;
                    case 0b100u:
                        return XORI;
                    case 0b110u:
                        return ORI;
                    case 0b111u:
                        return ANDI;
                    case 0b001u:
                        return SLLI;
                    case 0b101u: {
                        switch (funct7) {
                            case 0b0000000u:
                                return SRLI;
                            case 0b0100000u:
                                return SRAI;
                        }
                    }
                }
            }
            case 0b0110011u: {
                switch (funct3) {
                    case 0b000u: {
                        switch (funct7) {
                            case 0b0000000u:
                                return ADD;
                            case 0b0100000u:
                                return SUB;
                        }
                    }
                    case 0b001u:
                        return SLL;
                    case 0b010u:
                        return SLT;
                    case 0b011u:
                        return SLTU;
                    case 0b100u:
                        return XOR;
                    case 0b101u: {
                        switch (funct7) {
                            case 0b0000000u:
                                return SRL;
                            case 0b0100000u:
                                return SRA;
                        }
                    }
                    case 0b110u:
                        return OR;
                    case 0b111u:
                        return AND;
                }
            }
            default:
                return NOPE;
        }
    }

    uint getRS1(uint op) {
        return (op >> 15) & 0b11111u;
    }

    uint getRS2(uint op) {
        return (op >> 20) & 0b11111u;
    }

    uint getRD(uint op) {
        return (op >> 7) & 0b11111u;
    }

    uint getSHAMT(uint op) {
        return (op >> 20) & 0b11111u;
    }

    uint getIMM(uint op, opType ty) {
        uint imm = 0;
        uint inst31 = op >> 31;
        switch (ty) {
            case LUI:
            case AUIPC:
                imm = op & 0xfffff000u;
                return imm;
            case JAL:
                imm += (op >> 12) & 0xffu;
                imm <<= 1;
                imm += (op >> 20) & 0b1u;
                imm <<= 10;
                imm += (op >> 21) & 0x3ffu;
                imm <<= 1;
                if (inst31 == 0b1u) imm += (0xfffu << 20);
//                imm += (op >> 12) & 0xffu;
//                imm <<= 12;
//                imm += ((op >> 20) & 0b1u) << 11;
//                imm += (op >> 21) & 0x3ffu;
//                if (inst31 == 0b1u) imm += (0x3ffu << 20);
                return imm;
            case BEQ:
            case BNE:
            case BLT:
            case BGE:
            case BLTU:
            case BGEU:
                imm += (op >> 25) & 0x3fu;
                imm <<= 4;
                imm += (op >> 8) & 0xfu;
                imm <<= 1;
                imm += (((op >> 7) & 1u) << 11);
                if (inst31 == 1u) imm += (0xfffffu << 12);
                return imm;
            case LB:
            case LH:
            case LW:
            case LBU:
            case LHU:
            case ADDI:
            case SLTI:
            case SLTIU:
            case XORI:
            case ORI:
            case ANDI:
                if (inst31 == 1u) imm += (0xfffffu << 12);
                imm += (op >> 20);
                return imm;
            case SB:
            case SH:
            case SW:
                if (inst31 == 1u) imm += (0xfffffu << 7);
                imm += (op >> 25);
                imm <<= 5;
                imm += (op >> 7) & 0b11111u;
                return imm;
            default:
                return 0;
        }
    }
}

#define RISCSIMULATOR_PARSER_H

#endif //RISCSIMULATOR_PARSER_H
