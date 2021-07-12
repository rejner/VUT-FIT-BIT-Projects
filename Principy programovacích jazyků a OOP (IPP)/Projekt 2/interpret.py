import argparse
import xml.dom.minidom as dom
from interpret_lib.type_tests import *
from xml.parsers.expat import ExpatError
from interpret_lib.strings import *


def parse_arguments():

    arg_parser = argparse.ArgumentParser(description="Interpreter for XML representation of IPPcode20 language\n")
    arg_parser.add_argument('--source', help='File with XML representation of IPPcode20 language.')
    arg_parser.add_argument('--input', help='File with source code written in IPPcode20 language.')
    args = arg_parser.parse_args()

    if args.input:
        input_arg = args.input
    else:
        input_arg = ''
        
    if args.source:
        source_arg = args.source
    else:
        source_arg = ''

    if not args.input and not args.source:
        arg_parser.print_help()
        exit(10)

    return input_arg, source_arg


def parse_xml(src_file):

    # parse XML into dom document object
    try:
        if src_file != '':
            xml_src = dom.parse(src_file)
        else:
            xml_src = dom.parse(sys.stdin)

        root_attributes = list(xml_src.documentElement.attributes.items())
        
        # check correct root node
        if xml_src.documentElement.tagName != "program":
            exit(32)
        elif len(root_attributes) > 3:  # too much optional attributes
            exit(32)
        else:
            # check if program root element contains only legal attributes
            for attribute in root_attributes:
                if attribute[0] == "language":
                    continue
                elif attribute[0] == "name":
                    continue
                elif attribute[0] == "description":
                    continue
                else:
                    exit(32)

        # get elements of type <instruction>
        instructions = xml_src.getElementsByTagName("instruction")

    except ExpatError as msg:
        print(msg, file=sys.stderr)
        exit(31)

    except FileNotFoundError as msg:
        print(msg, file=sys.stderr)
        exit(11)

    # for each instruction tag:
    for instruction in instructions:

        # check number of attributes (only 2 attributes are legal)
        attributes = list(instruction.attributes.items())
        if len(attributes) != 2:
            exit(32)

        command = ''
        order = instruction.getAttribute("order")
        op_code = instruction.getAttribute("opcode")

        # check if opcode contains something
        if op_code == '':
            exit(32)

        # check if order contains something and is not negative or zero
        try:
            if order == '' or int(order) <= 0:
                exit(32)
        except ValueError as err:  # error in integer cast, order is not a number
            exit(32)

        command += order + " "
        command += op_code.upper() + " "

        arg1_found = False
        arg2_found = False
        arg3_found = False

        arg1 = None
        arg2 = None
        arg3 = None

        for arg in instruction.childNodes:
            # skip text instances
            if isinstance(arg, dom.Text):
                continue
            # arg is actual argument tag
            else:
                # argument tagName can only be arg1 | arg2 or arg3
                if arg.tagName == "arg1" or arg.tagName == "arg2" or arg.tagName == "arg3":

                    aux = arg.childNodes
                    if len(aux) != 1:
                        exit(32)

                    # check for correct order of arguments
                    if arg.tagName == "arg1":
                        if arg1 is None:
                            arg1_found = True
                            arg1 = str(arg.getAttribute("type")) + " "
                            try:
                                arg1 += str(arg.firstChild.nodeValue) + " "
                            except AttributeError:
                                # '#' = placeholder for no value (since # can't occur in program by default)
                                arg1 += '#' + " "
                        else:
                            exit(32)  # duplicite argument

                    elif arg.tagName == "arg2":
                        if arg2 is None:
                            arg2_found = True
                            arg2 = str(arg.getAttribute("type")) + " "
                            try:
                                arg2 += str(arg.firstChild.nodeValue) + " "
                            except AttributeError:
                                # '#' = placeholder for no value (since # can't occur in program by default)
                                arg2 += '#' + " "
                        else:
                            exit(32)  # duplicite argument

                    elif arg.tagName == "arg3":
                        if arg3 is None:
                            arg3_found = True
                            arg3 = str(arg.getAttribute("type")) + " "
                            try:
                                arg3 += str(arg.firstChild.nodeValue) + " "
                            except AttributeError:
                                # '#' = placeholder for no value (since # can't occur in program by default)
                                arg3 += '#' + " "
                        else:
                            exit(32)  # duplicite argument

                else:
                    # tagName is incorrect
                    exit(32)

        # check argument order
        if not (arg1_found or (arg1_found and arg2_found) or (arg1_found and arg2_found and arg3_found)):
            # might be instruction withnou parameters too
            if not (not arg1_found and not arg2_found and not arg3_found):
                exit(32)  # bad argument order

        if arg1 is not None:
            command += arg1
        if arg2 is not None:
            command += arg2
        if arg3 is not None:
            command += arg3

        # if any declaration of label was found, write it into label list
        if op_code.upper() == "LABEL":
            label = command.split()[3]
            for lab in label_list:
                if lab[0] == label:
                    exit(52)
            label_list.append([label, order])

        command = command.split()  # split command for further use
        program.append(command)  # append command to program data structure

    program.sort(key=lambda x: int(x[0]))  # sort instructions by order

    i = 0
    for line in program:
        if int(line[0]) == i-1:
            exit(32)  # elements are ordered and thus there must be duplicit order which is and error
        if int(line[0]) != i:
            program.insert(i, [f'{i}', "PASS"])
        i += 1

    global program_counter  # using program_counter from global scope
    while program_counter != len(program):
        execute_program(program[program_counter])
        program_counter += 1


# command: [0] - order
#          [1] - op_code
#          [2] - operand 1 type
#          [3] - operand 1 (destination)
#          [4] - operand 2 type
#          [5] - operand 2 (op1)
#          [6] - operand 3 type
#          [7] - operand 3 (op2)
def execute_program(command):

    global tmp_frame_exist
    global tmp_frame
    command = validate_command(command)

    order = command[0]
    op_code = command[1]

    if op_code == "DEFVAR":
        if len(command) != 4:
            exit(32)
        define_variable(command)

    elif op_code == "MOVE":
        if len(command) != 6:
            exit(32)
        assign_to_variable(command[3], [command[4], command[5]])

    elif op_code == "PUSHS":
        if len(command) != 4:
            exit(32)
        val = [command[2], command[3]]
        if command[2] == "var":
            val = get_variable_value(command[3])
        data_stack.append(val)

    elif op_code == "POPS":
        if len(command) != 4:
            exit(32)
        try:
            val = data_stack.pop()
        except IndexError:
            exit(56)
        assign_to_variable(command[3], [val[0], val[1]])

    elif op_code == "WRITE":
        if len(command) != 4:
            exit(32)
        if command[2] == "var":
            value = get_variable_value(command[3])
            if value[0] != "nil":
                if value[0] == "string":
                    value[1] = convert_special_chars(value[1])
                print(value[1], end='')
        else:
            if command[2] != "nil":
                if command[2] == "string":
                    command[3] = convert_special_chars(command[3])
                print(command[3], end='')

    elif op_code == "CREATEFRAME":
        if len(command) != 2:
            exit(32)
        # creates new temporary frame -> overwrites everything in current TF
        tmp_frame.clear()
        tmp_frame_exist = True

    elif op_code == "PUSHFRAME":
        if len(command) != 2:
            exit(32)
        if not tmp_frame_exist:
            exit(55)
        # push copy of tmp_frame onto top of the frame_stack stack
        frame_stack.append(tmp_frame.copy())
        tmp_frame_exist = False

    elif op_code == "POPFRAME":
        if len(command) != 2:
            exit(32)
        tmp_frame.clear()
        try:
            tmp_frame = list(frame_stack.pop())
        except IndexError:
            exit(55)
        tmp_frame_exist = True

    elif op_code == "JUMP":
        if len(command) != 4:
            exit(32)
        perform_jump(command[3])

    elif op_code == "JUMPIFEQ":
        if len(command) != 8:
            exit(32)
        op1, op2 = extract_operands(command)
        compatible_operands(op1, op2)
        if op1 == op2:
            perform_jump(command[3])

    elif op_code == "JUMPIFNEQ":
        if len(command) != 8:
            exit(32)
        op1, op2 = extract_operands(command)
        compatible_operands(op1, op2)
        if op1 != op2:
            perform_jump(command[3])

    # perform arithmetic operation
    elif op_code == "ADD" or op_code == "SUB" or op_code == "MUL" or op_code == "IDIV":
        if len(command) != 8:
            exit(32)
        calculate_arithmetic(order, op_code, command[3], [command[4], command[5]], [command[6], command[7]])

    elif op_code == "CALL":

        # expecting 4 elements in command structure
        if len(command) != 4:
            exit(32)

        global program_counter
        # push current incremented program counter value onto call stack
        call_stack.append(program_counter)
        # search for label and index for jump
        for label in label_list:
            if command[3] == label[0]:
                program_counter = int(label[1])
                return
        # label was not found -> not defined
        exit(52)

    elif op_code == "RETURN":
        if len(command) != 2:
            exit(32)
        try:
            # pop saved program counter value and assign its value to global program counter
            program_counter = call_stack.pop()
        except IndexError as e:
            # stack is empty --> value missing in the call stack
            print(e, file=sys.stderr)
            exit(56)

    elif op_code == "EQ" or op_code == "GT" or op_code == "LT" or op_code == "AND" or op_code == "OR":
        if len(command) != 8:
            exit(32)
        evaluate_condition(op_code, command[3], [command[4], command[5]], [command[6], command[7]])

    elif op_code == "NOT":
        if len(command) != 6:
            exit(32)
        op_value = extract_operand(command)
        if op_value[0] == "bool":
            op_value = convert_bool(op_value[1])
            result = not op_value
            result = convert_bool(result)
            assign_to_variable(command[3], ["bool", result])
        else:
            exit(53)

    elif op_code == "INT2CHAR":
        if len(command) != 6:
            exit(32)
        op_value = extract_operand(command)
        if op_value[0] == "int":
            try:
                char = str(chr(int(op_value[1])))
                assign_to_variable(command[3], ["string", char])
            except ValueError:
                exit(58)
        else:
            exit(53)

    elif op_code == "STRI2INT":
        if len(command) != 8:
            exit(32)
        op_value, index = extract_operands(command)
        test_string(order, op_value)
        test_int(order, index)

        if int(index[1]) < 0:
            exit(58)

        # convert string into list with indices
        array = list(op_value[1])
        try:
            result = ord(array[int(index[1])])
            assign_to_variable(command[3], ["int", result])
        except ValueError:
            exit(58)
        except IndexError:
            exit(58)

    elif op_code == "READ":
        if len(command) != 6:
            exit(32)
        # if file with inputs was provided, read data from this file, else expect data on STDIN
        if INPUT_FILE != '':
            in_val = f_input.readline()
            if in_val == '':  # there is nothing else to read -> result is nil
                assign_to_variable(command[3], ["nil", "nil"])
                return
            in_val = in_val.strip()  # newline character must be removed
        else:
            in_val = input().strip()

        val = validate_input(command[5], in_val)
        assign_to_variable(command[3], [val[0], val[1]])

    elif op_code == "CONCAT":
        if len(command) != 8:
            exit(32)
        op1_string, op2_string = extract_operands(command)

        test_string(order, op2_string)
        test_string(order, op1_string)

        result = str(op1_string[1]) + str(op2_string[1])
        assign_to_variable(command[3], ["string", result])

    elif op_code == "STRLEN":
        if len(command) != 6:
            exit(32)
        string = extract_operand(command)
        test_string(order, string)
        result = len(string[1])
        assign_to_variable(command[3], ["int", result])

    elif op_code == "GETCHAR":
        if len(command) != 8:
            exit(32)
        string, index = extract_operands(command)
        test_string(order, string)
        test_int(order, index)

        # check for negative index
        if int(index[1]) < 0:
            exit(58)

        string_array = list(string[1])
        try:
            result = string_array[int(index[1])]
            assign_to_variable(command[3], ["string", result])
        except IndexError:
            exit(58)

    elif op_code == "SETCHAR":
        if len(command) != 8:
            exit(32)
        string = get_variable_value(command[3])
        test_string(order, string)
        string_array = list(string[1])

        index, char = extract_operands(command)
        test_int(order, index)
        test_string(order, char)

        # test for correct index
        if int(index[1]) < 0:
            exit(58)

        char_array = list(char[1])
        try:
            string_array[int(index[1])] = char_array[0]
        except IndexError:
            exit(58)
        final_string = "".join(string_array)

        assign_to_variable(command[3], ["string", final_string])

    elif op_code == "TYPE":
        if len(command) != 6:
            exit(32)
        op_value = extract_operand(command, True)
        if op_value == [None, None]:
            op_type = ""
        else:
            op_type = op_value[0]

        assign_to_variable(command[3], ["string", op_type])

    elif op_code == "EXIT":
        if len(command) != 4:
            exit(32)
        err_code = [command[2], command[3]]
        if command[2] == "var":
            err_code = get_variable_value(command[3])
        test_int(order, err_code)
        if 0 <= int(err_code[1]) <= 49:
            exit(int(err_code[1]))
        else:
            exit(57)

    elif op_code == "DPRINT":
        if len(command) != 4:
            exit(32)
        value = [command[2], command[3]]
        if command[2] == "var":
            value = get_variable_value(command[3])
        print(value[1], file=sys.stderr)

    elif op_code == "BREAK":
        if len(command) != 2:
            exit(32)
        print_debug_info()

    # this instruction occurs only when continuous sequential sequence of order was broken and indices had to be filled
    elif op_code == "PASS":
        pass

    # label is already taken care of
    elif op_code == "LABEL":
        pass

    # unknown instruction
    else:
        exit(32)


# defines new variable in a frame
def define_variable(command):

    if len(command) != 4:
        exit(32)

    var = command[3].split('@')

    if var[0] == "GF":

        # search if variable already exists in global frame
        for variable in global_frame:
            if list(variable.keys()) == [f'{var[1]}']:
                exit(52)  # variable is already defined

        # append new variable to global frame
        global_frame.append({var[1]: [None, None]})

    elif var[0] == "LF":
        # get length of entire stack of frames to calculate index of the stack's top
        stack_top = len(frame_stack)-1

        # frame stack is empty
        if stack_top < 0:
            exit(55)

        # search if variable already exists in local frame
        if len(frame_stack[stack_top]) == 0:
            pass
        else:
            for variable in frame_stack[stack_top]:
                if list(variable.keys()) == [f'{var[1]}']:
                    exit(52)

        # append new variable to stack's top frame
        frame_stack[stack_top].append({var[1]: [None, None]})

    elif var[0] == "TF":
        if not tmp_frame_exist:
            exit(55)
        # search if variable already exists in temporary frame
        for variable in tmp_frame:
            if list(variable.keys()) == [f'{var[1]}']:
                exit(52)  # variable is already defined

        # append new variable to current temporary frame
        tmp_frame.append({var[1]: [None, None]})

    else:
        exit(32)


# assigns value to a specified variable
def assign_to_variable(variable, value):
    variable = variable.split('@')

    if value[0] == "label":
        exit(32)

    # we need to extract value from some variable
    if value[0] == "var":
        value = get_variable_value(value[1])

    if value[0] == "string":
        value[1] = convert_special_chars(value[1])

    index = 0
    if variable[0] == "GF":
        # search for variable in global_frame list
        for var in global_frame:
            if list(var.keys()) == [f'{variable[1]}']:
                global_frame[index][f'{variable[1]}'] = value
                return
            index += 1

    elif variable[0] == "TF":
        if not tmp_frame_exist:
            exit(55)
        # search for variable in tmp_frame list
        for var in tmp_frame:
            if list(var.keys()) == [f'{variable[1]}']:
                tmp_frame[index][f'{variable[1]}'] = value
                return
            index += 1

    elif variable[0] == "LF":
        stack_top = len(frame_stack)-1
        # frame stack is empty
        if stack_top < 0:
            exit(55)
        # search for variable in the top of local frame's stack
        for var in frame_stack[stack_top]:
            if list(var.keys()) == [f'{variable[1]}']:
                frame_stack[stack_top][index][f'{variable[1]}'] = value
                return
            index += 1

    # variable doesn't exist (Error 54)
    exit(54)


# finds and returns value of requested variable
def get_variable_value(variable, called_type=False):
    variable = variable.split('@')

    index = 0
    if variable[0] == "GF":
        # search for variable in global_frame list
        for var in global_frame:
            if list(var.keys()) == [f'{variable[1]}']:
                # test for missing value
                if global_frame[index][f'{variable[1]}'] == [None, None] and not called_type:
                    exit(56)
                return global_frame[index][f'{variable[1]}']
            index += 1

    elif variable[0] == "TF":
        if not tmp_frame_exist:
            exit(55)
        for var in tmp_frame:
            if list(var.keys()) == [f'{variable[1]}']:
                # test for missing value
                if tmp_frame[index][f'{variable[1]}'] == [None, None] and not called_type:
                    exit(56)
                return tmp_frame[index][f'{variable[1]}']
            index += 1

    elif variable[0] == "LF":
        stack_top = len(frame_stack)-1
        # frame stack is empty
        if stack_top < 0:
            exit(55)
        for var in frame_stack[stack_top]:
            if list(var.keys()) == [f'{variable[1]}']:
                # test for missing value
                if frame_stack[stack_top][index][f'{variable[1]}'] == [None, None] and not called_type:
                    exit(56)
                return frame_stack[stack_top][index][f'{variable[1]}']
            index += 1
    exit(54)


def calculate_arithmetic(order, operation, destination, op1, op2):

    # print(operation + " " + destination + " " + str(op1) + " " + str(op2))
    # check and extract values of operands
    if op1[0] == "var":
        op1 = get_variable_value(op1[1])

    if op2[0] == "var":
        op2 = get_variable_value(op2[1])

    test_int(order, op1)
    test_int(order, op2)

    result = 0
    try:
        if operation == "ADD":
            result = int(op1[1]) + int(op2[1])
        elif operation == "SUB":
            result = int(op1[1]) - int(op2[1])
        elif operation == "MUL":
            result = int(op1[1]) * int(op2[1])
        elif operation == "IDIV":
            try:
                result = int(op1[1]) // int(op2[1])
            except ZeroDivisionError:
                exit(57)
    # any number is not in correct integer format
    except ValueError as err:
        exit(32)

    # assign computed result to destination variable
    assign_to_variable(destination, ["int", result])


# evaluating conditions (LT/GT/EQ/AND/OR/NOT)
def evaluate_condition(operation, destination, op1, op2):

    if op1[0] == "var":
        op1 = get_variable_value(op1[1])

    if op2[0] == "var":
        op2 = get_variable_value(op2[1])

    if op1[0] == "string":
        op1[1] = convert_special_chars(op1[1])
    if op2[0] == "string":
        op2[1] = convert_special_chars(op2[1])

    op1_type = op1[0]
    op2_type = op2[0]

    if op1_type != op2_type:
        if (op1_type == "nil" or op2_type == "nil") and operation == "EQ":
            if op1_type == "nil":
                op1[1] = None
            if op2_type == "nil":
                op2[1] = None
        else:
            print("incompatible operand types", file=sys.stderr)
            exit(53)

    if op1_type == "nil" and op2_type == "nil" and operation != "EQ":
        exit(53)

    if op1_type == "int":
        op1[1] = int(op1[1])
    if op2_type == "int":
        op2[1] = int(op2[1])

    result = None
    if operation == "LT":
        if op1[1] < op2[1]:
            result = "true"
        else:
            result = "false"

    elif operation == "GT":
        if op1[1] > op2[1]:
            result = "true"
        else:
            result = "false"

    elif operation == "EQ":
        if op1[1] == op2[1]:
            result = "true"
        else:
            result = "false"

    # if result was found, assign it to variable
    if result:
        assign_to_variable(destination, ["bool", result])
        return
    # we need to test for basic boolean operations too
    else:

        if op1_type == "bool" and op2_type == "bool":
            # converting formats: "true"/"false" -> True/False  <--->  True/False -> "true"/"false"
            op1[1] = convert_bool(op1[1])
            op2[1] = convert_bool(op2[1])

            if operation == "AND":
                result = op1[1] and op2[1]
            elif operation == "OR":
                result = op1[1] or op2[1]

            # convert result into correct form
            result = convert_bool(result)

            # assign result to variable
            assign_to_variable(destination, ["bool", result])


def perform_jump(label):
    # search for label and index for jump
    for element in label_list:
        if label == element[0]:
            global program_counter
            # set program counter, this will perform jump
            program_counter = int(element[1])
            return

    # if no such label was found, exit 52
    exit(52)


# extract value from operand (for operations operating with only one symbol)
def extract_operand(command, called_type=False):
    operand = [command[4], command[5]]
    if command[4] == "var":
        if not called_type:
            operand = get_variable_value(command[5])
        else:
            operand = get_variable_value(command[5], True)
    if operand[0] == "string":
        operand[1] = convert_special_chars(operand[1])
    return operand


# extract values from two operands (for operations operating with two symbols)
def extract_operands(command):

    operand_1 = [command[4], command[5]]
    operand_2 = [command[6], command[7]]

    if command[4] == "var":
        operand_1 = get_variable_value(command[5])
    else:
        if operand_1[0] == 'int':
            operand_1 = [command[4], int(command[5])]

    if command[6] == "var":
        operand_2 = get_variable_value(command[7])
    else:
        if operand_2[0] == 'int':
            operand_2 = [command[6], int(command[7])]

    if operand_1[0] == "string":
        operand_1[1] = convert_special_chars(operand_1[1])
    if operand_2[0] == "string":
        operand_2[1] = convert_special_chars(operand_2[1])

    return operand_1, operand_2


def convert_bool(value):

    # test for program required format (String)
    if value == "true":
        return True
    elif value == "false":
        return False
    # test for intern boolean format (Bool)
    elif value:
        return "true"
    elif not value:
        return "false"
    else:
        # when called in boolean context, value must be in a wrong type
        exit(53)


def print_debug_info():
    print("---------------------------", file=sys.stderr)
    print("STATES OF DATA STRUCTURES: ", file=sys.stderr)
    print("---------------------------", file=sys.stderr)
    print("GLOBAL FRAME: " + str(global_frame), file=sys.stderr)
    print("TMP_FRAME: " + str(tmp_frame), file=sys.stderr)
    print("FRAME STACK: " + str(frame_stack), file=sys.stderr)
    print("DATA_STACK: " + str(data_stack), file=sys.stderr)
    print("LABEL LIST: " + str(label_list), file=sys.stderr)
    print("CALL STACK: " + str(call_stack), file=sys.stderr)
    print("PROGRAM COUNTER: " + str(program_counter), file=sys.stderr)
    print("___________________________", file=sys.stderr)


if __name__ == "__main__":

    # program_counter
    program_counter = 1

    # structure of program
    program = [['0', 'START']]

    # list representing global frame
    global_frame = []

    # temporary frame
    tmp_frame = []
    tmp_frame_exist = False

    # stack of local frames
    frame_stack = []

    # stack for storing data (PUSH and POP instructions)
    data_stack = []

    # list of labels in format: [{'label_name : order'}]
    label_list = []

    # stack for storing values of program counter (operation CALL)
    call_stack = []

    INPUT_FILE, SOURCE_FILE = parse_arguments()
    if INPUT_FILE != '':
        try:
            f_input = open(INPUT_FILE)
        except OSError as error:
            print(error, file=sys.stderr)
            exit(10)

    # print(INPUT_FILE + " | " + SOURCE_FILE)
    parse_xml(SOURCE_FILE)

    if INPUT_FILE != '':
        f_input.close()
    # print_debug_info()




