import sys

if len(sys.argv) != 3:
    print("USAGE: python3 assembler.py [assembly filename input] [name of binary output]")
    sys.exit(1)

byte_count = 0
labels = {}


try:
    with open(sys.argv[1], 'r') as asm_file:
        for raw_line in asm_file:
            line = raw_line.split(";", 1)[0].strip() # strip comments
            if not line or not raw_line: # skip empty lines 
                continue
            

            
except FileNotFoundError:
    print(f"{sys.argv[1]} file not found")
    sys.exit(1)

# first pass: remove comments, count bytes, create label table

for i in range(len(lines)):
    curr_line = lines[i]
    curr_line = curr_line.split(";", 1)[0] # remove comments
    if curr_line.endswith(":"):
        labels[curr_line.partition(':')[0]] = byte_count
    byte_count += 1
    if curr_line: 
        lines[i] = curr_line
    else:
        lines.pop(i)





program = bytes([0b11101000, 0b11111111, 0b11100010, 0xDF, 0b11100000, 0x0C, 0b00010010, 0b00100000, 0b01010000, 0b01000100, 0x05, 0b00001111])



filename = sys.argv[1]


