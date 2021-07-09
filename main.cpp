#include <iostream>
#include "MemoryBox.h"
#include "Operator.h"
#include "Parser.h"
#include "MyCpu.h"

using namespace Mzu;

inline void write(uint x) {
    if ((int) x < 0) {
        putchar('-');
        x = -x;
    }
    if (x > 9) write(x / 10);
    putchar(x % 10 + 48);
}

int main() {
    MzCPU cpu;
//    std::cout << "Hello, World!" << std::endl;
//    freopen("../test.data", "r", stdin) ;
    write(cpu.run());
    return 0;
}
