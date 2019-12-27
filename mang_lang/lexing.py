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

    def has_meaning(self) -> bool:
        return self.type != TokenType.WHITE_SPACE and \
               self.type != TokenType.NEW_LINES

class FixedParser:
    def __init__(self, token_type: TokenType):
        self.token_type = token_type

    def __call__(self, slice: Slice) -> Tuple[Token, Slice]:
        value = ''
        for _ in self.token_type.value:
            value += slice.pop()
        return (Token(type=self.token_type, value=value), slice)

def parse_string(slice: Slice) -> Tuple[Token, Slice]:
    assert slice.front() == '\"'
    value = ''
    value += slice.pop()
    while slice and slice.front() != '\"':
        value += slice.pop()
    value += slice.pop()
    return (Token(type=TokenType.STRING, value=value), slice)

def parse_number(slice: Slice) -> Tuple[Token, Slice]:
    value = ''
    while slice and slice.front() in '-+.1234567890':
        value += slice.pop()
    assert value
    return (Token(type=TokenType.NUMBER, value=value), slice)

def parse_symbol(slice: Slice) -> Tuple[Token, Slice]:
    value = ''
    while slice and (slice.front().isalnum() or slice.front() == '_'):
        value += slice.pop()
    assert value
    return (Token(type=TokenType.SYMBOL, value=value), slice)
