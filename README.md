# 8-Bit Computer Architecture and Instruction Set

## Abstract
The goal of this project is to design and implement an 8 bit computer architecture, designing all parts from scratch. The final product will be an 8 bit computer, simulated in FPGA logic using SystemVerilog, with a custom instruction set and assembly language to render it fully programmable, as well as an assembler which will convert the high level assembly into byte code which can be fed directly into the computer’s virtual memory. I have also made an emulator for this architecture written in C++, used to verify the architecture and debug before committing to hardware, as well as enabling faster iteration of the assembler.

## Overview of Machine
* An 8-bit PC (program counter) register points to the next instruction address.
* An 8-bit IR (instruction register) holds the current instruction.
* There are eight 8-bit GPR registers, accessible by a 3-bit address line.
* An 8-bit ACC register is used for results of computations.
* An 8-bit FLAG register contains flags `[0, 0, 0, 0, SIGN, ZERO, CARRY, OVERFLOW]`.
* The flag register is frequently written as `[S,Z,C,V]` hereafter.
* An 8-bit SP (Stack Pointer) register points to the next available stack slot.
* The SP is initialized to the top of the stack segment (Address `0xFF`) at reset.
* The stack grows downward from the top of memory.
* The GPRO register is used as a memory address pointer, with LD and ST instructions using the 8-bit value in GPRO as a memory address.
* There are 256 (2^8) words of r/w memory, with program and execution sharing the address space.
* Instructions are 8 bits, with the upper 4 bits denoting the opcode, and the lower 4 bits for the operand.

---

## Instruction Set Table

| Number | Opcode, Operand | Name | Flags | Meaning |
| :--- | :--- | :--- | :--- | :--- |
| 0 | `0000hhhh` | NOP | None | No operation |
| 1 | `0001drrr` | MA | None | Memory Access |
| 2 | `0010drrr` | ID | S, Z, V | Increment/Decrement |
| 3 | `0011mmmm` | JMPS | None | Jump if mmmm & flags != 0 (2 bytes) |
| 4 | `0100mmmm` | JMPC | None | Jump if mmmm & flags == 0 (2 bytes) |
| 5 | `0101drrr` | CPT | S, Z, C, V (CMP)<br>S, Z (TEST) | Compare/Test |
| 6 | `0110drrr` | AS | S, Z, C, V | Add/Subtract |
| 7 | `0111xxxd` | ASI | S, Z, C, V | Add/Subtract Immediate (2 bytes) |
| 8 | `1000xxxd` | NT | S, Z | Ones Complement/Twos Complement |
| 9 | `1001drrr` | ANOR | S, Z<br>C, V Cleared | And/Or |
| 10 | `1010trrr` | XOR | S, Z<br>C, V Cleared | Exclusive or |
| 11 | `1011xasd` | RTSH | S, Z, C | Shift/Rotate |
| 12 | `1100drrr` | SO | None | Stack Operation (Push/Pop) |
| 13 | `1101xxxd` | CR | None | Call/Return |
| 14 | `1110rrrr` | LDI | S, Z if ACC is targeted | Load immediate (2 bytes) |
| 15 | `1111drrr` | MVR | S, Z if ACC is modified | Move register |

---

## Instruction Details

### `0000hhhh` (NOP)
* **Name:** No Operation
* **Flags Updated:** None
* **Description:** NOP consumes a clock cycle and performs no operation. If `hhhh = 1111` it halts, otherwise it performs no operation.

### `0001drrr` (MA)
* **Name:** Memory Access
* **Flags Updated:** None
* **Description:** Memory Access is a complex instruction that enables store and load actions between GPR1-7 and the 256-byte internal memory, as well as the Accumulator (ACC) and the internal memory. GPRO is used as the address pointer which points to the address in the internal memory to be written to/from. In the MA instruction, the `rrr` field maps `000` to ACC. Therefore, GPRO cannot be the source or destination of a Memory Access, as it is busy holding the address pointer.
* **Operands:**
    * `d`: Directional bit. `d=0` indicates STORE register -> mem[GPR0]. `d=1` indicates LOAD mem[GPR0] -> register.
    * `rrr`: Register Index. `rrr=000` denotes the Accumulator. `rrr=001-111` indexes GPR1-7.

### `0010drrr` (ID)
* **Name:** Increment/Decrement
* **Flags Updated:** Z, S, V
* **Description:** Increment/Decrement is an instruction that allows direct increment or decrement of any GPRr, meant to streamline manipulation of loop variables, pointers, etc.
* **Operands:**
    * `d`: Directional bit. `d=0` indicates increment, `d=1` indicates decrement.
    * `rrr`: Register Index. `rrr=000-111` indexes GPR0-GPR7.

### `0011mmmm` (JMPS)
* **Name:** Jump if Flag in Mask is Set (2 bytes)
* **Flags Updated:** None
* **Description:** Jump if Flag in Mask is set is the first of two complex jump instructions aimed to cover the full set of conditional jumps with only two opcodes. The JMPS instruction takes a 4-bit mask which the programmer can use to select flags to check for. The JMPS instruction will conditionally jump to the 8-bit address supplied in the second byte of the instruction, provided `(MASK & FLAGS) != 0`. Program execution will branch if ANY of the flags selected by the mask are set. The advantage of this approach is that it allows the programmer to check multiple flags at once, using OR logic.
* **Examples:** `JMPS Z 0x78` will jump to address `0x78` conditionally on the Z flag (`JMPS 0100 0x78`). `JMPS Z|C 0x78` will jump to address `0x78` if Z OR C flag is set.
* **Operand:** `mmmm` where `mmmm` is a mask of the Flags register, `SZCV` (Sign, Zero, Carry, Overflow).

### `0100mmmm` (JMPC)
* **Name:** Jump if Flags in Mask are Clear (2 bytes)
* **Flags Updated:** None
* **Description:** Jump if Flag in Mask is Clear is the second of two complex jump instructions aimed to cover the full set of conditional jumps with only two opcodes. The JMPC instruction takes a 4-bit mask which the programmer can use to select flags to check for. The JMPC instruction will conditionally jump to the 8-bit address supplied in the second byte of the instruction, provided `(MASK & FLAGS) == 0`. Program execution will branch if ALL of the flags selected by the mask are clear. The advantage of this approach is that it allows the programmer to check multiple flags at once, using OR logic. An unconditional jump to `addr` can be achieved by use of `JMPC 0 addr`, because `FLAGS & 0` will always evaluate to 0.
* **Examples:** `JMPC Z 0x78` will jump to address `0x78` conditionally if the Z flag is clear (`JMPC 0100 0x78`). `JMPC Z|C 0x78` will jump to address `0x78` if Z AND C flag are clear.
* **Operand:** `mmmm` where `mmmm` is a mask of the Flags register, `SZCV` (Sign, Zero, Carry, Overflow).

### `0101drrr` (CPT)
* **Name:** Compare/Test
* **Flags Updated:** S, Z, C, V (Compare Mode) | S, Z (Test Mode)
* **Description:** Compare (`d=0`) performs a subtraction (ACC - GPRr) and updates flags but does not change ACC. This allows arithmetic comparisons without destroying the value in ACC. Test (`d=1`) performs a logical AND (ACC & GPRr) and updates flags but does not change ACC. This allows logical comparisons without destroying the value in ACC.

### `0110drrr` (AS)
* **Name:** Add/Subtract
* **Flags Updated:** S, Z, C, V
* **Description:** Add/Subtract adds to or subtracts from ACC the value in any GPRr. 
* **Operands:** `d`: Directional bit. `d=0` indicates add mode, `d=1` indicates subtract mode. `rrr`: Register Index. `rrr=000-111` indexes GPR0-GPR7.
* **Flag Notes:**
    * Carry flag: Set on carry-out for Addition, Set on no-borrow for subtraction.
    * Overflow (Addition): if `((rs == as) && (ss != as))`
    * Overflow (Subtraction): if `((rs != as) && (ss != as))`

### `0111xxxd` (ASI)
* **Name:** Add/Subtract Immediate (2 bytes)
* **Flags Updated:** S, Z, C, V
* **Description:** Add/Subtract Immediate adds to or subtracts from ACC any immediate 8-bit value provided in the immediately succeeding byte.
* **Operands:** `d`: Directional bit. `d=0` indicates add mode, `d=1` indicates subtract mode. `x`: don't care.

### `1000xxxd` (COMP)
* **Name:** One's/Two's Complement
* **Flags Updated:** S, Z
* **Description:** Takes the one's or two's complement of ACC and stores the value in ACC.
* **Operands:** `d`: Directional bit. `d=0` indicates one's complement, `d=1` indicates two's complement. `x`: don't care.

### `1001drrr` (ANOR)
* **Name:** And/Or
* **Flags Updated:** S, Z (Flags Cleared: C, V)
* **Description:** Bitwise and/or of ACC and GPRr. Stores result in ACC.
* **Operands:** `d`: Directional bit. `d=0` indicates and mode, `d=1` indicates or mode. `rrr`: Register Index. `rrr=000-111` indexes GPR0-GPR7.

### `1010trrr` (XOR)
* **Name:** Exclusive Or/Bit Toggle
* **Flags Updated:** S, Z (Flags Cleared: C, V)
* **Description:** Bitwise XOR of ACC and GPRr. Also allows single bit toggle of ACC in toggle mode (`t=1`).
* **Operands:** * `t=0`: XOR. `rrr`: register index selector GPR0-7.
    * `t=1`: ACC Bit Toggle. `rrr`: select ACC bit 0-7 to toggle.

### `1011xasd` (RTSH)
* **Name:** Rotate/Shift Through Carry
* **Flags Updated:** S, Z, C
* **Description:** Shifts or rotates the value in ACC left or right. Bits shifted out of ACC are stored in C flag, and rotations are rotated through C flag.
* **Operands:**
    * `a`: Logical or Arithmetic Shift Mode (Unused for rotational operations). `a=0`: Logical mode. Displaced bits from shift operations will be filled with 0s. `a=1`: Arithmetic mode. Right shifts fill the empty bit with the original Sign Bit (MSB) to preserve the sign of the number. Left shifts fill with 0.
    * `s`: Shift or Rotate Mode. `s=0`: ACC is rotated through the carry in the chosen direction. `s=1`: ACC is shifted logically or arithmetically in the chosen direction.
    * `d`: Direction Bit. `d=0`: The operation is performed to the left. `d=1`: The operation is performed to the right.
    * `x`: Don't Care.

### `1100drrr` (SO)
* **Name:** Stack Operation
* **Flags Updated:** None
* **Description:** Push or pop a value from GPRr (`r=1-7`) to the stack. Updates SP (Stack Pointer) automatically to maintain stack integrity.
* **Operands:**
    * `d`: direction bit. `d=0`: PUSH register r onto stack. `d=1`: POP top of stack into register r.
    * `rrr`: register index. `rrr=000` selects ACC, `001-111` selects GPR1-7. GPRO cannot be pushed/popped directly.

### `1101xxxd` (CR)
* **Name:** Call/Return (1 or 2 bytes)
* **Flags Updated:** None
* **Description:** Call (`d=0`) requires the following byte in code memory to hold the intended address to jump to. The next Program Counter (`PC+1`) value will be pushed onto the stack. Any registers must be pushed to the stack manually to save their state. Return (`d=1`) pops the top of the stack to the Program Counter (PC) to restore execution to a previous state.

### `1110rrrr` (LDI)
* **Name:** Load Immediate (2 bytes)
* **Flags Updated:** None unless target is accumulator (ACC), in which case S and Z will be updated accordingly.
* **Description:** Load an immediate 8-bit value into any GPRr register or ACC directly. Value will immediately follow instruction in program memory.
* **Operands:** `rrrr`: Register Index. `rrrr=0000-0111` indexes GPR0-GPR7. `1000` Denotes ACC. `1001-1111` currently unused.

### `1111drrr` (MVR)
* **Name:** Move Register
* **Flags Updated:** None
* **Description:** Loads the contents of any GPRr to ACC, or stores the value of ACC in any GPRr.
* **Operands:** `d`: Directional bit. `d=0` moves ACC to GPRr. `d=1` moves GPRr to ACC. `rrr`: Register Index. `rrr=000-111` indexes GPR0-GPR7.

---

## Example Program with Final State and Memory Dump

```text
LDI ACC, 0xFF;0,1
LDI GPR2, 0xDF ;2,3
LDI GPRO, 0x0C ;3,4
MA mem [GPRO], GPR2;5
ID I, GPRO  6
CPT GPR0;7
JMPC 060100 0x05 ;Z 8, 9
HALT ;10

0b11101000, 0611111111,
0611100010, 0xDF,
0b11100000,  ,
0600010010,
0600100000,
0601010000,
0601000100, 0x05,
0600001111

STATE DUMP
Program Counter: [12]
Instruction Register: [00001111]
Stack Pointer: [255]
Flag Register: [00000100]
Accumulator: [11111111]
GPRO: [11111111]
GPR1: [00000000]
GPR2: [11011111]
GPR3: [00000000]
GPR4: [00000000]
GPR5: [00000000]
GPR6: [00000000]
GPR7: [00000000]
Top of Stack: [11111111]

MEMORY DUMP
0xE8 0xFF 0xE2 0xDF 0xE0 0x0C 0x12 0x20 0x50 0x44 0x05 0x0F 0xDF 0xDF 0xDF 0xDF
0xDF 0xDF 0xDF 0xDF 0xDF 0xDF 0xDF 0xDF 8xDF 0xDF 0xDF 0xDF 0xDF 0xDF 0xDF 0xDF
0xDF 0xDF 0xDF 0xDF 0xDF 0xDF 0xDF 0xDF 0xDF 0xDF 0xDF 0xDF 0xDF 0xDF 0xDF 0xDF
0xDF 0xDF 0xDF 0xDF 0xDF 0xDF 0xDF 0xDF ... 0x00
