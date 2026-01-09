#include "cpu.h"
#include <stdint.h>


void CPU::h_alu(uint8_t a, uint8_t b, bool is_sub, bool update_ACC){ // computes a + b or a + (-b) and optionally stores the result in ACC

    const bool as = (a & 0b10000000); //a sign bit before operation, used for overflow detection
    const bool bs = (b & 0b10000000); //b sign bit before operation

    uint16_t sum; // make sum 16 bit so we can use bit 8 as the carry
    uint8_t result; // resulting value
    bool rs; // result sign

    clr_flags(FLAG_C | FLAG_V); // s and z are cleared in sz update function

    if (is_sub) sum = a + ~b + 1;
    else sum = a + b;

    result = ((uint8_t) sum & 0xFF); // mask out lsb 8 bits then cast instead of just truncating

    rs = (result & 0b10000000); //result sign bit AFTER operation

    // overflow flag V is difficult
    // overflow occurs when sign of operands don't match but sign of output differs from first operand FOR SIGNED SUBTRACTION

    const bool subtraction_overflow = ((bs != as) && (rs != as));
    const bool addition_overflow = ((bs == as) && (rs != as));

    if ((subtraction_overflow && is_sub) || (addition_overflow && !is_sub)) FLAGS |= FLAG_V;

    if (sum & 0x100) FLAGS |= FLAG_C;
    CPU::update_SZ_flags(result);

    if (update_ACC) ACC = result;
}

void CPU::update_SZ_flags(uint8_t reg){
    clr_flags(FLAG_S | FLAG_Z);
    if (reg & 0b10000000) FLAGS |= FLAG_S;
    if (reg == 0) FLAGS |= FLAG_Z;
}

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

    const bool is_dec = (operand & 0b1000); // check d bit, d=0 is increment, d=1 is decrement
    const uint8_t reg = (operand & 0b111); // mask out the register index, this time simple

    clr_flags(FLAG_S | FLAG_Z | FLAG_V);

    if (is_dec){
        GPR[reg]--;
        if (GPR[reg] == 0b01111111) FLAGS |= FLAG_V;
    } else {
        GPR[reg]++;
        if (GPR[reg] == 0b10000000) FLAGS |= FLAG_V;
    }

    CPU::update_SZ_flags(GPR[reg]);
    
}
void CPU::op_jmps(uint8_t operand, uint8_t address){
    // jmp if any flag in mask is set
    // operand mmmm mask (SZCV), 8 bit address

    const bool jmp = (operand & FLAGS);

    if (jmp) PC = address;

}
void CPU::op_jmpc(uint8_t operand, uint8_t address){
    // jump if all flags in mask are clear
    // operand mmmm mask (SZCV), 8 bit address

    bool jmp = (operand & FLAGS);
    jmp = !jmp;

    if (jmp) PC = address;
}
void CPU::op_cpt(uint8_t operand){
    // compare/test
    // operand drrr

    const bool is_test = (operand & 0b1000);
    const uint8_t reg = (operand & 0b111);

    if (is_test) { // logical AND, no ACC update
        clr_flags(FLAG_S | FLAG_Z);
        uint8_t res = (GPR[reg] & ACC);

        if (res == 0) FLAGS |= FLAG_Z;
        if (res & 0b10000000) FLAGS |= FLAG_S;
    } else { //cmp (arithmetic sub, no ACC update)
        h_alu(ACC, GPR[reg], true, false);
    }
}
void CPU::op_as(uint8_t operand){
    // add/subtract
    // operand drrr d=0 is add, d=1 is subtract

    const bool is_sub = operand & 0b1000;
    const uint8_t reg = operand & 0b111;

    h_alu(ACC, GPR[reg], is_sub, true);
}
void CPU::op_asi(uint8_t operand, uint8_t immediate){
    // add/subtract immediate
    // operand xxxd d=0 is add, d=1 is subtract

    const bool is_sub = operand & 0b1;

    h_alu(ACC, immediate, is_sub, true);
}
void CPU::op_nt(uint8_t operand){
    // not/complement
    // operand xxxd = 0 ones comp (not) d = 1 twos comp (complement)

    clr_flags(FLAG_S | FLAG_Z);

    const bool is_comp = operand & 0b1;

    if (is_comp) ACC = ~ACC + 1;
    else ACC = ~ACC;

    CPU::update_SZ_flags(ACC);

}
void CPU::op_anor(uint8_t operand){
    // and/or
    // operand drrr d = 0 AND, d = 1 OR

    clr_flags(FLAG_S | FLAG_Z);
    const bool is_or = operand & 0b1000;
    const uint8_t reg = operand & 0b111;

    if (is_or) ACC |= GPR[reg];
    else ACC &= GPR[reg];


    CPU::update_SZ_flags(ACC);

}
void CPU::op_xor(uint8_t operand){
    // xor/bit toggle
    // operand drrr rrr register select for xor, bit select for toggle

    const bool is_tog = operand & 0b1000;
    const uint8_t index = operand & 0b111;

    if (is_tog) ACC ^= (1<<index);
    else ACC ^= GPR[index];
    CPU::update_SZ_flags(ACC);
}
void CPU::op_rtsh(uint8_t operand){
    // rotate/shift
    //operand xasd 

    const bool is_ari = operand & 0b100;
    const bool is_rot = !(operand & 0b10);
    const bool is_right = operand & 0b1;

    const bool orig_carry = FLAGS & FLAG_C;

    clr_flags(FLAG_C | FLAG_S | FLAG_Z);

    if (is_right){
        const bool orig_sign = ACC & 0x80;
        const bool lsb = ACC & 0b1;
        ACC >>= 1;
        if (is_ari && !is_rot && orig_sign) ACC |= 0x80;
        if (is_rot && orig_carry) ACC |= 0x80;
        if (lsb) FLAGS |= FLAG_C;
    } else { //left
        const bool msb = ACC & 0x80;
        ACC <<= 1;
        if (is_rot && orig_carry) ACC |= 0b1;
        if (msb) FLAGS |= FLAG_C;
    }
    update_SZ_flags(ACC);
}
void CPU::op_so(uint8_t operand){
    // stack operation
    // operand drrr d=0 push d=1 pop
    // rrr = 000 targets ACC, 001-111 GPRr
    // sp points to unused top slot

    const bool is_pop = operand & 0b1000;
    const uint8_t reg = operand & 0b111;

    if (is_pop) {
        if (reg == 0){ // ACC target
            ACC = h_pop();
            update_SZ_flags(ACC);
        } else GPR[reg] = h_pop();
    }
    else {
        if (reg == 0) h_push(ACC);
        else h_push(GPR[reg]);
    }
}
void CPU::op_cr(uint8_t operand, uint8_t address){
    // call/return
    // operand xxxd d=0 CALL, d=1 RET

    const bool is_ret = operand & 0b1;

    if (is_ret) PC = h_pop();
    else {
        h_push(PC);
        PC = address;
    }
}
void CPU::op_ldi(uint8_t operand, uint8_t immediate){
    // load immediate
    // operand rrrr 0000-0111 GPRr 1000 ACC

    const bool is_acc = operand & 0b1000;
    const uint8_t reg = operand & 0b111;

    if (operand > 0b1000) return; // 0b1001 - 0b1111 unused

    if (is_acc) {
        ACC = immediate;
        update_SZ_flags(ACC);
    } else GPR[reg] = immediate;
    
}

void CPU::op_mvr(uint8_t operand){
    // move register
    // operand drrr d=0 ACC -> GPRr d=1 GPRr -> ACC

    const bool to_ACC = operand & 0b1000;
    const uint8_t reg = operand & 0b111;

    if (to_ACC){
        ACC = GPR[reg];
        update_SZ_flags(ACC);
    } else GPR[reg] = ACC;
}