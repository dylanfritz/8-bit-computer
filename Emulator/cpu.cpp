#include "cpu.h"
#include <iostream>
#include <stdint.h>

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

    if (dec){
        GPR[reg]--;
        FLAGS.V = GPR[reg] == 0b01111111; //signed decrement caused err
    } else {
        GPR[reg]++;
        FLAGS.V = GPR[reg] == 0b10000000; //sign bit flipped wrong
    }

    FLAGS.S = GPR[reg] & 0b10000000;
    FLAGS.Z = GPR[reg] == 0;
    
}
void CPU::op_jmps(uint8_t operand){
    // jmp if any flag in mask is set
    //
}
void CPU::op_jmpc(uint8_t operand){
    // jump if all flags in mask are clear
}
void CPU::op_cpt(uint8_t operand){
    // compare/test
}
void CPU::op_as(uint8_t operand){
    // add/subtract
}
void CPU::op_asi(uint8_t operand){
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
void CPU::op_cr(uint8_t operand){
    // call/return
}
void CPU::op_ldi(uint8_t operand){
    // load immediate
}