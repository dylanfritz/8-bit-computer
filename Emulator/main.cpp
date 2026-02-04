#include "cpu.h"
#include <vector>
#include <iostream>


int main(int argc, char* argv[]){
    CPU cpu;
    cpu.state_dmp(true);

    if (argc != 2){
        std::cout << "Gotta call the emulator on a binary lol" << std::endl;
        return 1;
    }

    std::string target_binary = argv[1];

    bool loaded = cpu.ld_file(target_binary);
    if(!loaded) return 1;

    bool halted = false;



    while (!halted) {
        halted = cpu.step();
        cpu.state_dmp(true);
        //std::cin.get();
    }

    return 0;
}

