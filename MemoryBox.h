//
// Created by mzi12 on 2021/6/30.
//

#ifndef RISCSIMULATOR_MEMORYBOX_H
#define RISCSIMULATOR_MEMORYBOX_H

#include <iostream>
#include <cstring>
#include <cstdio>

namespace Mzu {
    typedef unsigned uint;
    typedef unsigned short ushort;
    typedef unsigned char uchar;

    uint hex_to_dec(const std::string &hexx) {
        uint decc = 0, len = hexx.length(), pos = 0;
        while (len--) {
            decc <<= 4;
            if (hexx[pos] <= '9' && hexx[pos] >= '0') decc += (hexx[pos++] - '0');
            else if (hexx[pos] <= 'F' && hexx[pos] >= 'A') decc += (hexx[pos++] - 'A' + 10);
        }
        return decc;
    }

    uint hex_to_dec_single(const char &hexx) {
        uint decc = 0;

        if (hexx <= '9' && hexx >= '0') decc += (hexx - '0');
        else if (hexx <= 'F' && hexx >= 'A') decc += (hexx - 'A' + 10);

        return decc;
    }

    class MemoryBox {
        static const uint mem_size = 500000;
        uint mem[mem_size];
    public:
        void init() {
            memset(mem, 0, sizeof(mem));
            char ch;
            std::string address;
            uint pos;
            while (std::cin >> ch) {
                if (ch == '@') {
                    std::cin >> address;
                    pos = hex_to_dec(address);
                } else {
                    uint op = hex_to_dec_single(ch);
                    std::cin >> ch;
                    mem[pos++] = (op << 4) + hex_to_dec_single(ch);
                }
            }
        }

    public:
        const uint &operator[](const uint &i) const {
            return mem[i];
        }

        uint &operator[](const uint &i) {
            return mem[i];
        }

        uint load(uint pos, uint len) {
            uint xx = 0;
            for (int i = 0; i < len; ++i) {
                xx += mem[pos + i] << (8 * i);
            }
            return xx;
        }

        void store(uint pos, uint val, uint len) {
            for (int i = 0; i < len; ++i) {
                mem[pos + i] = val & 0xff;
                val >>= 8;
            }
        }

#ifdef Debug

        void print() {
            for (int i = 0; i < mem_size; ++i) {
                uint tmp = load(i, 4);
                printf("%x %x ", i,tmp);
                std::cout << i << ' ' << mem[i] << std::endl;
            }
        }

#endif
    };
}


#endif //RISCSIMULATOR_MEMORYBOX_H
