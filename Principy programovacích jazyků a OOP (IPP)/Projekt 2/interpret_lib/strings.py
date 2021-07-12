"""
Library for converting and working with strings in IPPcode20 format
"""
import re


def convert_special_chars(string):
    # replace all escape sequence if found
    escapes = re.findall("\\\\\d\d\d", string)
    for escape in escapes:
        string = string.replace(escape, chr(int(escape.lstrip('\\'))))
    return string
