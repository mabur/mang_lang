from enum import Enum
from typing import Tuple
from slice import Slice

class TokenType(Enum):
    NUMBER = "[+-]?([0-9]+[.])?[0-9]+"
    ARRAY_BEGIN = "["
    ARRAY_END = "]"
    DICTIONARY_BEGIN = "{"
    DICTIONARY_END = "}"
    EQUAL = "="
    COMMA = ","
    IF = "if "
    THEN = "then "
    ELSE = "else "
    EACH = "each "
    FOR = "for "
    IN = "in "
    FROM = "from "
    TO = "to "
    OF = "of "
    SYMBOL = "[a-z_]\\w*"
    STRING = "\"[\w\s]*\""
    WHITE_SPACE = " "
    NEW_LINES = "\n"

class FixedParser:
    def __init__(self, token_type: TokenType):
        self.token_type = token_type

    def __call__(self, slice: Slice) -> Tuple[str, Slice]:
        value = ''
        for _ in self.token_type.value:
            value += slice.pop()
        return (value, slice)
