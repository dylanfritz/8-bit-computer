# 8-Bit Computer Architecture and Instruction Set

## Abstract
[cite_start]The goal of this project is to design and implement an 8-bit computer architecture, designing all parts from scratch[cite: 1, 2]. [cite_start]The final product will be an 8-bit computer, simulated in FPGA logic using SystemVerilog, with a custom instruction set and assembly language to render it fully programmable[cite: 3]. [cite_start]It also features an assembler which will convert the high-level assembly into bytecode which can be fed directly into the computer's virtual memory[cite: 3]. [cite_start]An emulator for this architecture written in C++ was also made, used to verify the architecture and debug before committing to hardware, as well as enabling faster iteration of the assembler[cite: 4].

## Overview of Machine
* [cite_start]An 8-bit PC (program counter) register points to the next instruction address[cite: 5, 6].
* [cite_start]An 8-bit IR (instruction register) holds the current instruction[cite: 7].
* [cite_start]There are eight 8-bit GPR registers, accessible by a 3-bit address line[cite: 8].
* [cite_start]An 8-bit ACC register is used for results of computations[cite: 9].
* [cite_start]An 8-bit FLAG register contains flags `[0, 0, 0, 0, SIGN, ZERO, CARRY, OVERFLOW]`[cite: 10].
* [cite_start]The flag register is frequently written as `[S,Z,C,V]` hereafter[cite: 11].
* [cite_start]An 8-bit SP (Stack Pointer) register points to the next available stack slot[cite: 12].
* [cite_start]The SP is initialized to the top of the stack segment (Address `0xFF`) at reset[cite: 13].
* [cite_start]The stack grows downward from the top of memory[cite: 14].
* [cite_start]The GPRO register is used as a memory address pointer, with LD and ST instructions using the 8-bit value in GPRO as a memory address[cite: 15].
* [cite_start]There are 256 (2^8) words of r/w memory, with program and execution sharing the address space[cite: 16].
* [cite_start]Instructions are 8 bits, with the upper 4 bits denoting the opcode, and the lower 4 bits for the operand[cite: 17].

---

## Instruction Set Table

| Number | Opcode, Operand | Name | Flags | Meaning |
| :--- | :--- | :--- | :--- | :--- |
| 0 | `0000hhhh` | NOP | None | [cite_start]No operation [cite: 19] |
| 1 | `0001drrr` | MA | None | [cite_start]Memory Access [cite: 19] |
| 2 | `0010drrr` | ID | S, Z, V | [cite_start]Increment/Decrement [cite: 19, 20] |
| 3 | `0011mmmm` | JMPS | None | [cite_start]Jump if mmmm & flags != 0 (2 bytes) [cite: 20] |
| 4 | `0100mmmm` | JMPC | None | [cite_start]Jump if mmmm & flags == 0 (2 bytes) [cite: 20] |
| 5 | `0101drrr` | CPT | S, Z, C, V (CMP)<br>S, Z (TEST) | [cite_start]Compare/Test [cite: 20] |
| 6 | `0110drrr` | AS | S, Z, C, V | [cite_start]Add/Subtract [cite: 20] |
| 7 | `0111xxxd` | ASI | S, Z, C, V | [cite_start]Add/Subtract Immediate (2 bytes) [cite: 20] |
| 8 | `1000xxxd` | NT | S, Z | [cite_start]Ones Complement/Twos Complement [cite: 20] |
| 9 | `1001drrr` | ANOR | S, Z<br>C, V Cleared | [cite_start]And/Or [cite: 20] |
| 10 | `1010trrr` | XOR | S, Z<br>C, V Cleared | [cite_start]Exclusive or [cite: 20] |
| 11 | `1011xasd` | RTSH | S, Z, C | [cite_start]Shift/Rotate [cite: 20] |
| 12 | `1100drrr` | SO | None | [cite_start]Stack Operation (Push/Pop) [cite: 20] |
| 13 | `1101xxxd` | CR | None | [cite_start]Call/Return [cite: 20] |
| 14 | `1110rrrr` | LDI | S, Z if ACC is targeted | [cite_start]Load immediate (2 bytes) [cite: 20] |
| 15 | `1111drrr` | MVR | S, Z if ACC is modified | [cite_start]Move register [cite: 20] |

---

## Instruction Details

### `0000hhhh` (NOP)
* [cite_start]**Name:** No Operation [cite: 22, 23]
* [cite_start]**Flags Updated:** None [cite: 24]
* [cite_start]**Description:** NOP consumes a clock cycle and performs no operation[cite: 25]. [cite_start]If `hhhh = 1111` it halts, otherwise it performs no operation[cite: 26].

### `0001drrr` (MA)
* [cite_start]**Name:** Memory Access [cite: 27]
* [cite_start]**Flags Updated:** None [cite: 28]
* [cite_start]**Description:** Memory Access is a complex instruction that enables store and load actions between GPR1-7 and the 256-byte internal memory, as well as the Accumulator (ACC) and the internal memory[cite: 29]. [cite_start]GPRO is used as the address pointer which points to the address in the internal memory to be written to/from[cite: 30]. [cite_start]In the MA instruction, the `rrr` field maps `000` to ACC[cite: 31]. [cite_start]Therefore, GPRO cannot be the source or destination of a Memory Access, as it is busy holding the address pointer[cite: 32].
* **Operands:**
    * `d`: Directional bit. `d=0` indicates STORE register -> mem[GPR0]. [cite_start]`d=1` indicates LOAD mem[GPR0] -> register[cite: 33].
    * `rrr`: Register Index. `rrr=000` denotes the Accumulator. [cite_start]`rrr=001-111` indexes GPR1-7[cite: 34].

### `0010drrr` (ID)
* [cite_start]**Name:** Increment/Decrement [cite: 35, 36]
* [cite_start]**Flags Updated:** Z, S, V [cite: 37]
* [cite_start]**Description:** Increment/Decrement is an instruction that allows direct increment or decrement of any GPRr, meant to streamline manipulation of loop variables, pointers, etc[cite: 38].
* **Operands:**
    * `d`: Directional bit. [cite_start]`d=0` indicates increment, `d=1` indicates decrement[cite: 39].
    * `rrr`: Register Index. [cite_start]`rrr=000-111` indexes GPR0-GPR7[cite: 40].

### `0011mmmm` (JMPS)
* [cite_start]**Name:** Jump if Flag in Mask is Set (2 bytes) [cite: 41, 42]
* [cite_start]**Flags Updated:** None [cite: 43]
* [cite_start]**Description:** Jump if Flag in Mask is set is the first of two complex jump instructions aimed to cover the full set of conditional jumps with only two opcodes[cite: 44]. [cite_start]The JMPS instruction takes a 4-bit mask which the programmer can use to select flags to check for[cite: 45]. [cite_start]The JMPS instruction will conditionally jump to the 8-bit address supplied in the second byte of the instruction, provided `(MASK & FLAGS) != 0`[cite: 46, 47]. [cite_start]Program execution will branch if ANY of the flags selected by the mask are set[cite: 47]. [cite_start]The advantage of this approach is that it allows the programmer to check multiple flags at once, using OR logic[cite: 49].
* [cite_start]**Examples:** `JMPS Z 0x78` will jump to address `0x78` conditionally on the Z flag (`JMPS 0100 0x78`)[cite: 48]. [cite_start]`JMPS Z|C 0x78` will jump to address `0x78` if Z OR C flag is set[cite: 50].
* [cite_start]**Operand:** `mmmm` where `mmmm` is a mask of the Flags register, `SZCV` (Sign, Zero, Carry, Overflow)[cite: 51].

### `0100mmmm` (JMPC)
* [cite_start]**Name:** Jump if Flags in Mask are Clear (2 bytes) [cite: 52, 53]
* [cite_start]**Flags Updated:** None [cite: 54]
* [cite_start]**Description:** Jump if Flag in Mask is Clear is the second of two complex jump instructions aimed to cover the full set of conditional jumps with only two opcodes[cite: 55]. [cite_start]The JMPC instruction takes a 4-bit mask which the programmer can use to select flags to check for[cite: 56]. [cite_start]The JMPC instruction will conditionally jump to the 8-bit address supplied in the second byte of the instruction, provided `(MASK & FLAGS) == 0`[cite: 57]. [cite_start]Program execution will branch if ALL of the flags selected by the mask are clear[cite: 57]. [cite_start]The advantage of this approach is that it allows the programmer to check multiple flags at once, using OR logic[cite: 59]. [cite_start]An unconditional jump to `addr` can be achieved by use of `JMPC 0 addr`, because `FLAGS & 0` will always evaluate to 0[cite: 61].
* [cite_start]**Examples:** `JMPC Z 0x78` will jump to address `0x78` conditionally if the Z flag is clear (`JMPC 0100 0x78`)[cite: 58]. [cite_start]`JMPC Z|C 0x78` will jump to address `0x78` if Z AND C flag are clear[cite: 60].
* [cite_start]**Operand:** `mmmm` where `mmmm` is a mask of the Flags register, `SZCV` (Sign, Zero, Carry, Overflow)[cite: 62].

### `0101drrr` (CPT)
* [cite_start]**Name:** Compare/Test [cite: 63]
* **Flags Updated:** S, Z, C, V (Compare Mode) | [cite_start]S, Z (Test Mode) [cite: 64]
* [cite_start]**Description:** Compare (`d=0`) performs a subtraction (ACC - GPRr) and updates flags but does not change ACC[cite: 65]. [cite_start]This allows arithmetic comparisons without destroying the value in ACC[cite: 66]. [cite_start]Test (`d=1`) performs a logical AND (ACC & GPRr) and updates flags but does not change ACC[cite: 67]. [cite_start]This allows logical comparisons without destroying the value in ACC[cite: 68].

### `0110drrr` (AS)
* [cite_start]**Name:** Add/Subtract [cite: 69]
* [cite_start]**Flags Updated:** S, Z, C, V [cite: 70]
* [cite_start]**Description:** Add/Subtract adds to or subtracts from ACC the value in any GPRr[cite: 71]. 
* **Operands:** `d`: Directional bit. `d=0` indicates add mode, `d=1` indicates subtract mode. `rrr`: Register Index. [cite_start]`rrr=000-111` indexes GPR0-GPR7[cite: 72].
* **Flag Notes:**
    * [cite_start]Carry flag: Set on carry-out for Addition, Set on no-borrow for subtraction[cite: 73, 74].
    * [cite_start]Overflow (Addition): if `((rs == as) && (ss != as))` [cite: 74, 75]
    * [cite_start]Overflow (Subtraction): if `((rs != as) && (ss != as))` [cite: 75]

### `0111xxxd` (ASI)
* [cite_start]**Name:** Add/Subtract Immediate (2 bytes) [cite: 77, 78]
* [cite_start]**Flags Updated:** S, Z, C, V [cite: 79]
* [cite_start]**Description:** Add/Subtract Immediate adds to or subtracts from ACC any immediate 8-bit value provided in the immediately succeeding byte[cite: 80].
* **Operands:** `d`: Directional bit. `d=0` indicates add mode, `d=1` indicates subtract mode. [cite_start]`x`: don't care[cite: 81].

### `1000xxxd` (COMP)
* [cite_start]**Name:** One's/Two's Complement [cite: 82, 83]
* [cite_start]**Flags Updated:** S, Z [cite: 84]
* [cite_start]**Description:** Takes the one's or two's complement of ACC and stores the value in ACC[cite: 85].
* **Operands:** `d`: Directional bit. `d=0` indicates one's complement, `d=1` indicates two's complement. [cite_start]`x`: don't care[cite: 86].

### `1001drrr` (ANOR)
* [cite_start]**Name:** And/Or [cite: 87, 88]
* [cite_start]**Flags Updated:** S, Z (Flags Cleared: C, V) [cite: 89, 90]
* **Description:** Bitwise and/or of ACC and GPRr. [cite_start]Stores result in ACC[cite: 91].
* **Operands:** `d`: Directional bit. `d=0` indicates and mode, `d=1` indicates or mode. `rrr`: Register Index. [cite_start]`rrr=000-111` indexes GPR0-GPR7[cite: 92].

### `1010trrr` (XOR)
* [cite_start]**Name:** Exclusive Or/Bit Toggle [cite: 93, 94]
* [cite_start]**Flags Updated:** S, Z (Flags Cleared: C, V) [cite: 95, 96]
* **Description:** Bitwise XOR of ACC and GPRr. [cite_start]Also allows single bit toggle of ACC in toggle mode (`t=1`)[cite: 97].
* **Operands:** * `t=0`: XOR. `rrr`: register index selector GPR0-7[cite: 98, 99].
    * `t=1`: ACC Bit Toggle. [cite_start]`rrr`: select ACC bit 0-7 to toggle[cite: 100, 101].

### `1011xasd` (RTSH)
* [cite_start]**Name:** Rotate/Shift Through Carry [cite: 102, 103]
* [cite_start]**Flags Updated:** S, Z, C [cite: 104]
* [cite_start]**Description:** Shifts or rotates the value in ACC left or right[cite: 105]. [cite_start]Bits shifted out of ACC are stored in C flag, and rotations are rotated through C flag[cite: 106].
* **Operands:**
    * [cite_start]`a`: Logical or Arithmetic Shift Mode (Unused for rotational operations)[cite: 107]. `a=0`: Logical mode. [cite_start]Displaced bits from shift operations will be filled with 0s[cite: 108]. `a=1`: Arithmetic mode. Right shifts fill the empty bit with the original Sign Bit (MSB) to preserve the sign of the number. [cite_start]Left shifts fill with 0[cite: 108, 109, 110].
    * [cite_start]`s`: Shift or Rotate Mode[cite: 111]. [cite_start]`s=0`: ACC is rotated through the carry in the chosen direction[cite: 112]. [cite_start]`s=1`: ACC is shifted logically or arithmetically in the chosen direction[cite: 113].
    * [cite_start]`d`: Direction Bit[cite: 114]. [cite_start]`d=0`: The operation is performed to the left[cite: 115]. [cite_start]`d=1`: The operation is performed to the right[cite: 116].
    * [cite_start]`x`: Don't Care[cite: 117].

### `1100drrr` (SO)
* [cite_start]**Name:** Stack Operation [cite: 118, 119]
* [cite_start]**Flags Updated:** None [cite: 120]
* **Description:** Push or pop a value from GPRr (`r=1-7`) to the stack. [cite_start]Updates SP (Stack Pointer) automatically to maintain stack integrity[cite: 121].
* **Operands:**
    * [cite_start]`d`: direction bit[cite: 122]. [cite_start]`d=0`: PUSH register r onto stack[cite: 123]. [cite_start]`d=1`: POP top of stack into register r[cite: 124].
    * `rrr`: register index. `rrr=000` selects ACC, `001-111` selects GPR1-7. [cite_start]GPRO cannot be pushed/popped directly[cite: 125].

### `1101xxxd` (CR)
* [cite_start]**Name:** Call/Return (1 or 2 bytes) [cite: 126, 127]
* [cite_start]**Flags Updated:** None [cite: 128]
* [cite_start]**Description:** Call (`d=0`) requires the following byte in code memory to hold the intended address to jump to[cite: 129]. [cite_start]The next Program Counter (`PC+1`) value will be pushed onto the stack[cite: 130]. [cite_start]Any registers must be pushed to the stack manually to save their state[cite: 131]. [cite_start]Return (`d=1`) pops the top of the stack to the Program Counter (PC) to restore execution to a previous state[cite: 132].

### `1110rrrr` (LDI)
* [cite_start]**Name:** Load Immediate (2 bytes) [cite: 133, 134]
* [cite_start]**Flags Updated:** None unless target is accumulator (ACC), in which case S and Z will be updated accordingly[cite: 135].
* [cite_start]**Description:** Load an immediate 8-bit value into any GPRr register or ACC directly[cite: 136]. [cite_start]Value will immediately follow instruction in program memory[cite: 137].
* **Operands:** `rrrr`: Register Index. `rrrr=0000-0111` indexes GPR0-GPR7. `1000` Denotes ACC. [cite_start]`1001-1111` currently unused[cite: 138].

### `1111drrr` (MVR)
* [cite_start]**Name:** Move Register [cite: 139, 140]
* [cite_start]**Flags Updated:** None [cite: 141]
* [cite_start]**Description:** Loads the contents of any GPRr to ACC, or stores the value of ACC in any GPRr[cite: 142].
* **Operands:** `d`: Directional bit. `d=0` moves ACC to GPRr. [cite_start]`d=1` moves GPRr to ACC[cite: 143]. `rrr`: Register Index. [cite_start]`rrr=000-111` indexes GPR0-GPR7[cite: 144].

---

## State and Memory Dump

```text
LDI ACC, 0xFF;0,1 [cite: 145]
LDI GPR2, 0xDF ;2,3 [cite: 145]
LDI GPRO, 0x0C ;3,4 [cite: 146]
MA mem [GPRO], GPR2;5 [cite: 147]
ID I, GPRO  6 [cite: 148]
CPT GPR0;7 [cite: 149]
JMPC 060100 0x05 ;Z 8, 9 [cite: 150]
HALT ;10 [cite: 151]

0b11101000, 0611111111, [cite: 152]
0611100010, 0xDF, [cite: 153]
0b11100000,  , [cite: 154]
0600010010, [cite: 155]
0600100000, [cite: 156]
0601010000, [cite: 157]
0601000100, 0x05, [cite: 158]
0600001111 [cite: 159]

STATE DUMP [cite: 160]
Program Counter: [12] [cite: 161]
Instruction Register: [00001111] [cite: 162]
Stack Pointer: [255] [cite: 163]
Flag Register: [00000100] [cite: 164]
Accumulator: [11111111] [cite: 165]
GPRO: [11111111] [cite: 166]
GPR1: [00000000] [cite: 167]
GPR2: [11011111] [cite: 168]
GPR3: [00000000] [cite: 169]
GPR4: [00000000] [cite: 170]
GPR5: [00000000] [cite: 171]
GPR6: [00000000] [cite: 172]
GPR7: [00000000] [cite: 173]
Top of Stack: [11111111] [cite: 174]

MEMORY DUMP [cite: 175]
0xE8 0xFF 0xE2 0xDF 0xE0 0x0C 0x12 0x20 0x50 0x44 0x05 0x0F 0xDF 0xDF 0xDF 0xDF
0xDF 0xDF 0xDF 0xDF 0xDF 0xDF 0xDF 0xDF 8xDF 0xDF 0xDF 0xDF 0xDF 0xDF 0xDF 0xDF
0xDF 0xDF 0xDF 0xDF 0xDF 0xDF 0xDF 0xDF 0xDF 0xDF 0xDF 0xDF 0xDF 0xDF 0xDF 0xDF
0xDF 0xDF 0xDF 0xDF 0xDF 0xDF 0xDF 0xDF ... 0x00 [cite: 176, 177, 178, 179]
