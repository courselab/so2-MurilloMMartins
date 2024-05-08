import  sys

def handle_hexadecimal_value(value):
    value = value.replace(',', '')
    value_size = len(value)

    if value_size > 4:
        first = value[:4]
        first_hex = int(first, 16)
        second = '0x' + value[4:]
        second_hex = int(second, 16)

        return bytes([second_hex,first_hex])

    hex_value = int(value.replace(',', ''), 16)
    return bytes([hex_value])

def handle_char_value(value: str):
    value = value.replace(',', '').replace('\'', '')
    return value.encode('utf-8')

def get_register_mov_opcode(register):
    if register == 'al':
        return bytes([176])
    elif register == 'ah':
        return bytes([180])

def mov_instruction(instructions, index):
    value = instructions[index+1].replace('$', '')
    register = instructions[index+2].replace('%', '')

    # we need to handle spaces
    if register[0] == '\'':
        value = '\' \','
        register = instructions[index+3].replace('%', '')

    prefix = value[0]
    if prefix == '0':
        value = handle_hexadecimal_value(value)
    if prefix == '\'':
        value = handle_char_value(value)

    register = get_register_mov_opcode(register)

    return register+value

def int_instruction(instructions, index):
    value = instructions[index+1].replace('$', '')
    value = handle_hexadecimal_value(value)

    opcode = bytes([205])
    
    return opcode+value

def jmp_instruction(instructions, index):
    # this is hardcoded, but can be implemented by keeping track of labels
    # and implementing two's complement
    return bytes([235, 253])

def hlt_instruction():
    return bytes([244])

def word_instruction(instructions, index):
    value = instructions[index+1].replace('$', '')
    value = handle_hexadecimal_value(value)

    return value

def main():
    argv = sys.argv
    input_filename = argv[1]
    output_filename = argv[2]

    input_file = open(input_filename, "r")
    output_file = open(output_filename, "wb")
    line = input_file.readline()
    while(line):
        line = line.rstrip()
        cnt = 0

        instructions = line.split()
        # print(instructions)
        for index in range(len(line.split())):
            instruction = instructions[index]

            # if we find a comment, we skip
            if instruction == '#':
                break
            elif instruction == 'mov':
                binary = mov_instruction(instructions, index)
                output_file.write(binary)
            elif instruction == 'int':
                binary = int_instruction(instructions, index)
                output_file.write(binary)
            elif instruction == 'hlt':
                binary = hlt_instruction()
                output_file.write(binary)
            elif instruction == 'jmp':
                binary = jmp_instruction(instructions, index)
                output_file.write(binary)
            elif instruction == '.fill':
                # auto-filling with zeros
                output_file.write(bytes(461))
            elif instruction == '.word':
                binary = word_instruction(instructions, index)
                output_file.write(binary)
            
            # print(instructions)
            cnt += 1

        line = input_file.readline()


    input_file.close()

if __name__ == '__main__':
    main()