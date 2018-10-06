from enum import Enum
import re
from typing import Sequence


class TokenType(Enum):
    NUMBER = "[+-]?([0-9]+[.])?[0-9]+"
    SYMBOL = "[a-z_]\\w*"
    PARENTHESIS_BEGIN = "\("
    COMMA = ","
    PARENTHESIS_END = "\)"
    EQUAL = "="
    NEW_LINE = "\n"
    BRACKET_BEGIN = "\["
    BRACKET_END = "\]"


class Token:
    def __init__(self, type: TokenType, value: str):
        self.type = type
        self.value = value

    def __len__(self) -> int:
        return len(self.value)


def lexer(code: str) -> Sequence[Token]:
    code = code.replace(' ', '')
    num_characters = len(code)
    index = 0
    tokens = []
    while index < num_characters:
        token = _match_token(code, index)
        tokens.append(token)
        index += len(token)
    return tokens

def _match_token(code: str, index: int) -> Token:
    for token_type in TokenType:
        regex = re.compile(token_type.value)
        match = regex.match(code, index)
        if match:
            return Token(type=token_type, value=match.group(0))
    raise "No matching token"
