#include <stdint.h>
#include <iostream>
#include <vector>

constexpr uint8_t FLAG_S = 0b00001000;
constexpr uint8_t FLAG_Z = 0b00000100;
constexpr uint8_t FLAG_C = 0b00000010;
constexpr uint8_t FLAG_V = 0b00000001;

class CPU {
    private:

        //state 
        uint8_t memory[256];
        uint8_t GPR[8];
        uint8_t PC;
        uint8_t ACC;
        uint8_t SP;
        uint8_t FLAGS;

        inline void clr_flags(uint8_t mask){
            FLAGS &= ~mask;
        }
        inline void h_push(uint8_t val){
            memory[SP--] = val;
        }
        inline uint8_t h_pop(){
            return memory[++SP];
        }

        void update_SZ_flags(uint8_t reg);

        void h_alu(uint8_t a, uint8_t b, bool is_sub, bool update_regs);


        void op_nop(uint8_t operand);
        void op_ma(uint8_t operand);
        void op_id(uint8_t operand);
        void op_jmps(uint8_t operand, uint8_t address);
        void op_jmpc(uint8_t operand, uint8_t address);
        void op_cpt(uint8_t operand);
        void op_as(uint8_t operand);
        void op_asi(uint8_t operand, uint8_t immediate);
        void op_nt(uint8_t operand);
        void op_anor(uint8_t operand);
        void op_xor(uint8_t operand);
        void op_rtsh(uint8_t operand);
        void op_so(uint8_t operand);
        void op_cr(uint8_t operand, uint8_t address);
        void op_ldi(uint8_t operand, uint8_t immediate);
        void op_mvr(uint8_t operand);

    public:
        void reset();
        void step();
        void state_dmp();
        void ld_program(const std::vector<uint8_t>& bytecode);



};