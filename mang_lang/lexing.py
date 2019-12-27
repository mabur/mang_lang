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

class Token:
    def __init__(self, type: TokenType, value: str):
        self.type = type
        self.value = value

    def __len__(self) -> int:
        return len(self.value)

class FixedParser:
    def __init__(self, token_type: TokenType):
        self.token_type = token_type

    def __call__(self, slice: Slice) -> Tuple[Token, Slice]:
        value = ''
        for _ in self.token_type.value:
            value += slice.pop()
        return (Token(type=self.token_type, value=value), slice)
