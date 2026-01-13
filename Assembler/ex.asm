NAMED CONSTANTS

FLAG_S ; 0b1000
FLAG_Z ; 0b0100
FLAG_C ; 0b0010
FLAG_V ; 0b0001


NUMBER FORMATS

Hexadecimal prefixed by 0x ; 0xF
Binary prefixed by 0b ; 0b1111
Decimal with no prefix ; 15


LABELS

Labels are used to define points in the program to jump to, without remembering specific addresses. 
A label can be any unique alphanumeric non-keyword string ending in a colon (:). Some example labels below.

Loop: 
Here1:
MyFunc:


INSTRUCTION USAGE

NOP (HALT)
NOP ; No operation
NOP HALT ; Halt

MA [RD|WR] [REG]
MA RD ACC ; Read the memory location pointed to by GPR0 into ACC.
MA WR ACC ; Write ACC to the memory location pointed to by GPR0.
MA RD R7 ; Read the memory location pointed to by GPR0 into GPR7.
MA WR R7 ; Write GPR7 to the memory location pointed to by GPR0.
MA WR R0 ; Invalid, GPR0 cannot be directly loaded or saved to memory.

ID [I|D] [REG]
ID I R0 ; Increment R0
ID D R0 ; Decrement R0

JMPS [MASK] [ADDR|LABEL]
JMPS 0xF 0xFF ; Conditionally jump to address 0xFF if any flags masked by 0xF (all flags) are set.
JMPS 0b1111 255 ; Same as above, different number formats. 
JMPS 0b1111 Loop ; Jump to lable Loop if any flag is set.
JMPS FLAG_Z Loop ; Jump to lable Loop if Z flag is set.
JMPS FLAG_C | FLAG_S Loop; Jump to lable loop if Z or C flag is set.

JMPC [MASK] [ADDR|LABEL]
JMPC 0xF 0xFF ; Conditionally jump to address 0xFF if all flags masked by 0xF (all flags) are clear.
JMPC 0b1111 255 ; Same as above, different number formats. 
JMPC 0 0b11111111 ; Unconditionally jump to address 0b11111111 (255)
JMPC 0 Loop ; Unconditionally jump to lable Loop
JMPC FLAG_Z Loop ; Jump to lable Loop if Z flag is clear.
JMPC FLAG_C | FLAG_S Loop; Jump to lable loop if Z AND C flag are clear. Note, this is unintuitive, but because the first operand of the JMPC instruction is a mask to check which flag bits are clear, this is the proper way to construct the mask using named constants for the flags.

CPT [C|T] [REG]
CPT C R0 ; Subtract R0 from ACC, updating flags but not modifying ACC (Compare R0)
CPT T R7 ; Logically AND R7 with ACC, updating flags but not modifying ACC (Test R7)

AS [A|S] [REG]
AS A R0 ; Add R0 to ACC
AS S R7 ; Subtract R7 from ACC

ASI [A|S] [IMMEDIATE_VALUE] 
ASI A 0xFF ; Add FF hex to ACC
ASI S 3 ; Subtract 3 decimal from ACC
ASI A 0b111 ; Add 111 binary to ACC

COMP [O|T]
COMP O ; One's Complement
COMP T ; Two's Complement

ANOR [A|O] [REG]
ANOR A R0 ; Logically AND GPR0 with ACC, and store the result in ACC.
ANOR O R6 ; Logically OR GPR6 with ACC, and store the result in ACC.

XOR [X|T] [REG|INDEX]
XOR X R6 ; Logically XOR GPR6 with ACC, and store the result in ACC.
XOR T 0b111 ; Bitwise toggle the 7th least significant bit in ACC, leaving other bits untouched.
XOR T 7; Bitwise toggle the 7th least significant bit in ACC, leaving other bits untouched.

RTSH [TYPE] [MODE] [DIRECTION]
RTSH [A|L] [R|S] [L|R]
RTSH L S R ; Logically shift right
RTSH A S R ; Arithmetically shift ACC right
RTSH A S L ; Invalid, arithmetic left shifts are invalid.
RTSH L S L ; Logical shift left
RTSH S L ; Logical shift left
RTSH S R ; Logical shift right
RTSH R L ; Rotate left through carry
RTSH R R ; Rotate right through carry

SO [PUSH|POP] [REG]
SO PUSH ACC ; Push ACC onto the stack
SO POP R0 ; Invalid, R0 cannot be pushed, popped to
SO POP R1 ; Pop the top of the stack into R1

CR [CALL|RET] (ADDR|LABEL)
CR CALL 0xFF ; Push the current program counter onto the stack and then jump to address 0xFF.
CR RET 0xFF ; Invalid
CR RET ; Pop the top of the stack into the program counter, jumping to a saved address.
CR CALL Loop ; Push the current program counter onto the stack and then jump to address denoted by Loop.

LDI [REG] [IMMEDIATE_VALUE]
LDI ACC 0xDF ; Load DF hex into ACC
LDI R0 32 ; Load 32 decimal into GPR0

MVR [DEST_REG] [SRC_REG]
MVR R0 ACC ; Store the contents of ACC into GPR0
MVR ACC R0 ; Load the ACC with the contents of GPR0


ASSEMBLER MACROS

HALT ; NOP HALT

RD [REG] ; MA RD [REG]
WR [REG] ; MA WR [REG]

INC [REG] ; ID I [REG]
DEC [REG] ; ID D [REG]

JMP [ADDR] ; JMPC 0 [ADDR]
JPS [ADDR] ; JMPS 0b1000 [ADDR]
JPZ [ADDR] ; JMPS 0b0100 [ADDR]
JPC [ADDR] ; JMPS 0b0010 [ADDR]
JPV [ADDR] ; JMPS 0b0001 [ADDR]

CP [REG] ; CPT C [REG]
TEST [REG] ; CPT T [REG]

ADD [REG] ; AS A [REG]
SUB [REG] ; AS S [REG]

ADDI [IMMEDIATE_VALUE] ; ASI A [IMMEDIATE_VALUE]
SUBI [IMMEDIATE_VALUE] ; ASI S [IMMEDIATE_VALUE]

AND [REG] ; ANOR A [REG]
OR [REG] ; ANOR O [REG]

TOG [INDEX] ;  XOR T [INDEX]

ASR ; RTSH A S R
LSR ; RTSH L S R
LSL ; RTSH L S L
RR ; RTSH R R
RL ; RTSH R L

PUSH [REG] ; SO PUSH [REG]
POP [REG] ; SO POP [REG]

CALL [ADDR|LABEL] ; CR CALL [ADDR|LABEL]
RET ; CR RET



