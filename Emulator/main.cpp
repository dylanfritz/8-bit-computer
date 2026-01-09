#include "cpu.h"
#include <vector>
#include <iostream>

std::vector<uint8_t> test = {0b11101000, 0b11111111, 0b11100010, 0xDF, 0b11100000, 0x0C, 0b00010010, 0b00100000, 0b01010000, 0b01000100, 0x05, 0b00001111};

int main(){
    CPU cpu;
    cpu.state_dmp(true);

    cpu.ld_mem(test);

    bool halted = false;



    while (!halted) {
        halted = cpu.step();
        cpu.state_dmp(true);
        //std::cin.get();
    }

    return 0;
}