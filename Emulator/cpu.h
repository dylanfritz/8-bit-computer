#include <stdint.h>
#include <iostream>
#include <vector>

class CPU {
    private:

        //state 
        uint8_t memory[256];
        uint8_t GPR[8];
        uint8_t PC;
        uint8_t ACC;
        uint8_t SP;

        struct {
            bool S, Z, C, V;
        } FLAGS;

        void op_nop(uint8_t operand);
        void op_ma(uint8_t operand);
        void op_id(uint8_t operand);
        void op_jmps(uint8_t operand);
        void op_jmpc(uint8_t operand);
        void op_cpt(uint8_t operand);
        void op_as(uint8_t operand);
        void op_asi(uint8_t operand);
        void op_nt(uint8_t operand);
        void op_anor(uint8_t operand);
        void op_xor(uint8_t operand);
        void op_rtsh(uint8_t operand);
        void op_so(uint8_t operand);
        void op_cr(uint8_t operand);
        void op_ldi(uint8_t operand);

    public:
        void reset();
        void step();
        void state_dmp();
        void ld_program(const std::vector<uint8_t>& bytecode);



};