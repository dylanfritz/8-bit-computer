#include "cpu.h"
#include <stdint.h>
#include <iostream>


void CPU::op_nop(uint8_t operand){
    // nop
}
void CPU::op_ma(uint8_t operand){
    // memory access
    // operand drrr 

    if (operand & 0b1000) { // d=1 mem->reg
        uint8_t reg = operand & 0b111;
        if (reg) GPR[reg] = memory[GPR[0]]; // rrr = 000 selects accumulator not gpr0
        else ACC = memory[GPR[0]];
    } else { // d=0 reg -> mem
        uint8_t reg = operand & 0b111;
        if (reg) memory[GPR[0]] = GPR[reg]; // rrr = 000 selects accumulator not gpr0
        else memory[GPR[0]] = ACC;
    }
}
void CPU::op_id(uint8_t operand){
    // increment decrement
    // operand drrr

    bool dec = (operand & 0b1000); // check d bit, d=0 is increment, d=1 is decrement
    uint8_t reg = (operand & 0b111); // mask out the register index, this time simple

    clr_flags(FLAG_S | FLAG_Z | FLAG_V);

    if (dec){
        GPR[reg]--;
        if (GPR[reg] == 0b01111111) FLAGS |= FLAG_V;
    } else {
        GPR[reg]++;
        if (GPR[reg] == 0b10000000) FLAGS |= FLAG_V;
    }

    if (GPR[reg] & 0b10000000) FLAGS |= FLAG_S;
    if (GPR[reg] == 0) FLAGS |= FLAG_Z;
    
}
void CPU::op_jmps(uint8_t operand, uint8_t address){
    // jmp if any flag in mask is set
    // operand mmmm mask (SZCV), 8 bit address

    bool jmp = (operand & FLAGS);

    if (jmp) {
        PC = address;
    }

}
void CPU::op_jmpc(uint8_t operand, uint8_t address){
    // jump if all flags in mask are clear
    // operand mmmm mask (SZCV), 8 bit address

    bool jmp = (operand & FLAGS);
    jmp = !jmp;

    if (jmp) {
        PC = address;
    }
}
void CPU::op_cpt(uint8_t operand){
    // compare/test
}
void CPU::op_as(uint8_t operand){
    // add/subtract
    // operand drrr d=0 is add, d=1 is subtract

    clr_flags(FLAG_S | FLAG_Z | FLAG_C |FLAG_V);

    bool sub = operand & 0b1000;
    uint8_t reg = operand & 0b111;

    uint16_t sum; // make sum 16 bit so we can use bit 8 as the carry

    if (sub) sum = ACC + ~(GPR[reg]) + 1;
    else sum = ACC + GPR[reg];

    if (sum & 0b100000000) FLAGS &= FLAG_C;
    if (!sum) FLAGS &= FLAG_Z;
    

}
void CPU::op_asi(uint8_t operand, uint8_t immediate){
    // add/subtract immediate
}
void CPU::op_nt(uint8_t operand){
    // not/complement
}
void CPU::op_anor(uint8_t operand){
    // and/or
}
void CPU::op_xor(uint8_t operand){
    // xor/bit toggle
}
void CPU::op_rtsh(uint8_t operand){
    // rotate/shift
}
void CPU::op_so(uint8_t operand){
    // stack operation
}
void CPU::op_cr(uint8_t operand, uint8_t address){
    // call/return
}
void CPU::op_ldi(uint8_t operand, uint8_t immediate){
    // load immediate
}