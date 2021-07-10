//
// Created by mzi12 on 2021/7/1.
//


//#define Debug
//#define Debug_Printer

#ifdef Debug

#include <cstdio>

#endif

#ifndef RISCSIMULATOR_MYCPU_H
using namespace Mzu;
namespace Mzu {
    class MzCPU {
        RegisterJar jar;
        MemoryBox box;
        uint pc;

        struct Predictor {
            uint success = 0, total = 0;
            int counter[4096];

            Predictor() {
                memset(counter, 0, sizeof(counter));
            }
        } pd;

        uint num_mem = 0u;

        struct IF_ID {
            bool isBusy = false;
            uint pc = 0u;
            uint code = 0u;
        } reg1;

        struct ID_EX {
//            int pd = 0u;在于pc是否改变
            bool isBusy = false;
            uint pc = 0u;
            opType type = NOPE;
            uint imm = 0u;
            uint rs1_data = 0u;
            uint rs2_data = 0u;
            uint rd = 0u;
        } reg2;

        struct EX_MEM {//todo simplify to former edition
            bool isBusy = false;
            opType type = NOPE;
            uint pos = 0u;
            uint pos_data = 0u;
            uint rd = 0u;
            uint rd_data = 0u;
        } reg3;

        struct MEM_WB {
            bool isBusy = false;
            opType type = NOPE;
            uint rd = 0u;
            uint rd_data = 0u;
        } reg4;

        void IF() {//InstructionFetch
            if (reg1.isBusy)
                return;
            reg1.pc = pc;
            reg1.code = box.load(pc, 4);
#ifdef Debug
            printf("%x\n", pc);
#endif
            pc += 4;
            reg1.isBusy = true;
        }

        void ID() {//InstructionDecode
            if (reg2.isBusy || !reg1.isBusy) return;
            //reg2.isBusy = false;reg1.isBusy = true;
            if (reg1.code == END) {
                reg1.isBusy = false;
                return;
            }

            uint op = reg1.code;
            opType type = getType(op);
            uint rs1 = getRS1(op);
            uint rs2 = getRS2(op);
            uint imm = getIMM(op, type);
            uint rd = getRD(op);

            if ((rs1 != 0u && jar.check(rs1)) || (rs2 != 0u && jar.check(rs2))) return;
#ifdef Debug_Printer
            std::cout << "reg1: " << type << std::endl;
#endif
            reg2.type = type;
            reg2.rd = rd;
            reg2.pc = reg1.pc;
            reg2.imm = imm;
            if (type != LUI && type != AUIPC && type != JAL && type != NOPE) {
                reg2.rs1_data = jar[rs1];
            }

            switch (type) {
                case JAL:
                    pc = reg1.pc + (int) reg2.imm;
                    break;
                case JALR:
                    pc = ((reg2.rs1_data + (int) reg2.imm) >> 1) << 1;
                    break;
                case SLLI:
                case SRLI:
                case SRAI:
                    reg2.imm = getSHAMT(op);
                    break;
                case BEQ:
                case BNE:
                case BLT:
                case BGE:
                case BLTU:
                case BGEU:
                case SB:
                case SH:
                case SW:
                case ADD:
                case SUB:
                case SLL:
                case SLT:
                case SLTU:
                case XOR:
                case SRL:
                case SRA:
                case OR:
                case AND:
                    reg2.rs2_data = jar[rs2];
                default:
                    break;
            }
            switch (type) {
                case BEQ:
                case BNE:
                case BLT:
                case BGE:
                case BLTU:
                case BGEU:
//                     if (pd.counter[(reg2.pc >> 2u) & 0b111111u] & 0b10u) pc = reg2.pc + reg2.imm;
//                     else pc = reg2.pc + 4;
//                     ++pd.total;
//                    reg2.pd = pd.counter[(reg1.pc >> 2u) & 0b111111u] & 0b10u;
                case SB:
                case SH:
                case SW:
                case NOPE:
                    reg2.rd = 0u;
            }
            //forwarding
            if (reg3.rd)
                if (reg3.rd == rs1 || reg3.rd == rs2) return;

            if (reg2.rd != 0u)
                jar.check(reg2.rd) = true;
            reg1.isBusy = false;
            reg2.isBusy = true;
        }

        void EX() {//Execute
            if (!reg2.isBusy || reg3.isBusy) return;
            reg2.isBusy = false;
            reg3.type = reg2.type;
#ifdef Debug_Printer
            std::cout << "reg2: " << reg2.type << std::endl;
#endif
            reg3.rd = reg2.rd;
            reg3.isBusy = true;
            bool flag = false;
            switch (reg3.type) {
                case LUI:
                    reg3.rd_data = reg2.imm;
                    break;
                case AUIPC:
                    reg3.rd_data = reg2.imm + reg2.pc;
                    break;
                case JAL:
                case JALR:
                    reg3.rd_data = reg2.pc + 4;
                    break;
                case BEQ:
                    if (reg2.rs1_data == reg2.rs2_data) flag = true;
                    break;
                case BNE:
                    if (reg2.rs1_data != reg2.rs2_data) flag = true;
                    break;
                case BLT:
                    if ((int) reg2.rs1_data < (int) reg2.rs2_data) flag = true;
                    break;
                case BGE:
                    if ((int) reg2.rs1_data >= (int) reg2.rs2_data) flag = true;
                    break;
                case BLTU:
                    if (reg2.rs1_data < reg2.rs2_data) flag = true;
                    break;
                case BGEU:
                    if (reg2.rs1_data >= reg2.rs2_data) flag = true;
                    break;
                case LB:
                case LH:
                case LW:
                case LBU:
                case LHU:
                    reg3.pos = reg2.rs1_data + reg2.imm;
                    break;
                case SB:
                case SH:
                case SW:
                    reg3.pos_data = reg2.rs2_data;
                    reg3.pos = reg2.rs1_data + reg2.imm;
                    break;
                case ADDI:
                    reg3.rd_data = reg2.rs1_data + reg2.imm;
                    break;
                case SLTI:
                    reg3.rd_data = (int) reg2.rs1_data < (int) reg2.imm;
                    break;
                case SLTIU:
                    reg3.rd_data = (reg2.rs1_data < reg2.imm);
                    break;
                case XORI:
                    reg3.rd_data = reg2.rs1_data ^ reg2.imm;
                    break;
                case ORI:
                    reg3.rd_data = reg2.rs1_data | reg2.imm;
                    break;
                case ANDI:
                    reg3.rd_data = reg2.rs1_data & reg2.imm;
                    break;
                case SLLI:
                    reg3.rd_data = reg2.rs1_data << reg2.imm;
                    break;
                case SRLI:
                    reg3.rd_data = reg2.rs1_data >> reg2.imm;
                    break;
                case SRAI:
                    int tmp;
                    tmp = (int) reg2.rs1_data >> reg2.imm;
                    reg3.rd_data = (uint) tmp;
                    break;
                case ADD:
                    reg3.rd_data = reg2.rs1_data + reg2.rs2_data;
                    break;
                case SUB:
                    reg3.rd_data = reg2.rs1_data - reg2.rs2_data;
                    break;
                case SLL:
                    reg3.rd_data = reg2.rs1_data << reg2.rs2_data;
                    break;
                case SLT:
                    reg3.rd_data = (int) reg2.rs1_data < (int) reg2.rs2_data;
                    break;
                case SLTU:
                    reg3.rd_data = reg2.rs1_data < reg2.rs2_data;
                    break;
                case XOR:
                    reg3.rd_data = reg2.rs1_data ^ reg2.rs2_data;
                    break;
                case SRL:
                    reg3.rd_data = reg2.rs1_data >> reg2.rs2_data;
                    break;
                case SRA:
                    int tmp1;
                    tmp1 = (int) reg2.rs1_data >> reg2.rs2_data;
                    reg3.rd_data = (uint) tmp1;
                    break;
                case OR:
                    reg3.rd_data = reg2.rs1_data | reg2.rs2_data;
                    break;
                case AND:
                    reg3.rd_data = reg2.rs1_data & reg2.rs2_data;
                    break;
                case NOPE:
                    break;
            }
            switch (reg3.type) {
                case BEQ:
                case BNE:
                case BLT:
                case BGE:
                case BLTU:
                case BGEU: {
                    if (flag) {
                         pc = reg2.pc + reg2.imm;
//                         if (((pd.counter[(reg2.pc & 0b1111111u)] & 0b10u) >> 1) == 1) ++pd.success;
//                         else {
//                             pc = reg2.pc + reg2.imm;
//                             reg1.isBusy = false;
//                         }
//                         pd.counter[reg2.pc & 0b1111111u] = min(pd.counter[(reg2.pc >> 2u) & 0b111111u] + 1, 3);
//                     } else {
//                         if (((pd.counter[(reg2.pc & 0b1111111u)] & 0b10u) >> 1) == 1) {
//                             pc = reg2.pc + 4;
//                             reg1.isBusy = false;
//                         } else {
//                             ++pd.success;
//                         }
//                         pd.counter[(reg2.pc >> 2u) & 0b111111u] = max(pd.counter[(reg2.pc >> 2u) & 0b111111u] - 1,
//                                                                            0);
//                     }
                }
            }

            reg2.type = NOPE;
            reg2.pc = reg2.rd = reg2.rs1_data = reg2.rs2_data = reg2.imm = 0u;
        }

        void MEM() {//Memory Access
            if (!reg3.isBusy || reg4.isBusy) return;

            if (num_mem < 3u) {
                ++num_mem;
                return;
            }

            num_mem = 0u;
            reg3.isBusy = false;
            reg4.type = reg3.type;
#ifdef Debug_Printer
            std::cout << "reg3: " << reg3.type << std::endl;
#endif
            reg4.rd = reg3.rd;
            reg4.isBusy = true;
            switch (reg3.type) {
                case BEQ:
                    break;
                case BNE:
                    break;
                case BLT:
                    break;
                case BGE:
                    break;
                case BLTU:
                    break;
                case BGEU:
                    break;
                case LB:
                    char data_b;
                    data_b = (char) box.load(reg3.pos, 1);
                    reg4.rd_data = (uint) data_b;
                    break;
                case LH:
                    short data_h;
                    data_h = (short) box.load(reg3.pos, 2);
                    reg4.rd_data = (uint) data_h;
                    break;
                case LW:
                    int data_w;
                    data_w = (int) box.load(reg3.pos, 4);
                    reg4.rd_data = (uint) data_w;
                    break;
                case LBU:
                    uchar data_bu;
                    data_bu = (uchar) box.load(reg3.pos, 1);
                    reg4.rd_data = (uint) data_bu;
                    break;
                case LHU:
                    ushort data_hu;
                    data_hu = (ushort) box.load(reg3.pos, 2);
                    reg4.rd_data = (uint) data_hu;
                    break;
                case SB:
                    box.store(reg3.pos, reg3.pos_data, 1);
                    break;
                case SH:
                    box.store(reg3.pos, reg3.pos_data, 2);
                    break;
                case SW:
                    box.store(reg3.pos, reg3.pos_data, 4);
                    break;
                case NOPE:
                    break;
                default:
                    reg4.rd_data = reg3.rd_data;
            }
            reg3.type = NOPE;
            reg3.pos = reg3.pos_data = reg3.rd = reg3.rd_data = 0u;
        }

        void WB() {//Write Back
            if (!reg4.isBusy) return;
#ifdef Debug_Printer_Jar
            printReg();
#endif
            reg4.isBusy = false;
            if (reg4.rd == 0) return;
#ifdef Debug_Printer
            if (reg4.rd != 0) std::cout << "reg4: " << reg4.type << std::endl;
#endif
            switch (reg4.type) {
                case BEQ:
                case BNE:
                case BLT:
                case BGE:
                case BLTU:
                case BGEU:
                case SB:
                case SH:
                case SW:
                case NOPE:
                    return;//break;
                default:
                    if (reg4.rd != 0) {
                        jar.check(reg4.rd) = false;

                        jar[reg4.rd] = reg4.rd_data;
#ifdef Debug_Printer
                        printReg();
#endif
                    }
                    break;
            }

            reg4.rd_data = reg4.rd = 0u;
            reg4.type = NOPE;
        }

#ifdef Debug_Printer

        void printReg() {
            std::cout << "--------printReg" << std::endl;
            for (int i = 0; i < 32; ++i) {
                std::cout << i << ' ' << jar[i] << std::endl;
            }
            std::cout << "----------------" << std::endl;
        }

        void printJar() {
            std::cout << "--------printReg" << std::endl;
            for (int i = 0; i < 32; ++i) {
                std::cout << i << ' ' << jar.check(i) << std::endl;
            }
            std::cout << "----------------" << std::endl;
        }

#endif

    public:
        uint run() {
            pc = 0u;
            box.init();
//#ifdef Debug
//                        box.print();
//#endif
//            while (true) {
//                IF();
//                if (reg1.code == END) break;
//                ID();
//                EX();
//                MEM();
//                WB();
//#ifdef Debug
//                printReg();
//#endif
//            }
//            return jar[10] & 0xff;
#ifdef Debug
            uint i = 0u;
#endif
#ifdef Debug_Printer
            while (reg1.code != END) {
#ifdef Debug
                ++i;
                if (i == 354)
                    break;
#endif
                WB();
                MEM();
                EX();
                ID();
                IF();
                if (reg1.code == END && !reg1.isBusy && !reg2.isBusy && !reg3.isBusy && !reg4.isBusy) break;
#ifdef Debug
                std::cout << "---------------print busy" << std::endl;
                std::cout << reg1.isBusy << ' ' << reg2.isBusy << ' ' << reg3.isBusy << ' ' << reg4.isBusy << std::endl;
                std::cout << "---------------print end" << std::endl;
#endif
            }
#endif
            while (reg1.code != END) {
#ifdef Debug
                ++i;
                std::cout << "***********   " << i << "   **********" << std::endl;
#endif
                WB();
                MEM();
                EX();
                ID();
                IF();
                if (reg1.code == END && !reg1.isBusy && !reg2.isBusy && !reg3.isBusy && !reg4.isBusy) break;
#ifdef Debug
                std::cout << "---------------print busy" << std::endl;
                std::cout << reg1.isBusy << ' ' << reg2.isBusy << ' ' << reg3.isBusy << ' ' << reg4.isBusy << std::endl;
                std::cout << "---------------print end" << std::endl;
#endif
            }
            return jar[10] & 0xff;
        }
    };
}
#define RISCSIMULATOR_MYCPU_H

#endif //RISCSIMULATOR_MYCPU_H
