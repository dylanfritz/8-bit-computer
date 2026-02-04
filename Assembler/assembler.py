import sys

if len(sys.argv) != 3:
    print("USAGE: python3 assembler.py [assembly filename input] [name of binary output]")
    sys.exit(1)


program = bytes([0b11101000, 0b11111111, 0b11100010, 0xDF, 0b11100000, 0x0C, 0b00010010, 0b00100000, 0b01010000, 0b01000100, 0x05, 0b00001111])



filename = sys.argv[1]



with open(filename, "wb") as f:
    f.write(program)
