"""
Library for testing type of value
"""
import sys


def test_int(order, value):
    if not value[0] == "int":
        print_error(order, value[0], "int")
        exit(53)


def test_string(order, value):
    if not value[0] == "string":
        print_error(order, value[0], "string")
        exit(53)


def test_type(order, value):
    if not value[0] == "type":
        print_error(order, value[0], "type")
        exit(53)


def test_var(order, value):
    if not value[0] == "var":
        print_error(order, value[0], "var")
        exit(53)


def test_label(order, value):
    if not value[0] == "label":
        print_error(order, value[0], "label")
        exit()


def compatible_operands(op1, op2):
    if op1[0] != op2[0]:
        if op1[0] == "nil" or op2[0] == "nil":
            return
        exit(53)


def validate_command(command):

    # convert special values and escape sequences to its values
    command = [part.replace('#', '').replace('\\010', '\n').replace('\\032', " ") for part in command]

    """
    escapes = re.findall(r'\\\\\d\d\d', command)
    for escape in escapes:
        command = command.replace(escape, chr(int(escape.lstrip('\\'))))
    """
    return command


def validate_input(value_type, value):

    if value_type == "bool":
        value = value.lower()
        if value != "true":
            value = "false"

    if value_type == "int":
        try:
            value = int(value)
        except ValueError:
            # not an integer
            value_type = "nil"
            value = "nil"

    return [value_type, value]


def print_error(order, received, required):
    if not received:
        received = "None"

    print("[" + order + "] " + "Incompatible operand types, received " + received + ", required " + required +
          "!", file=sys.stderr)
