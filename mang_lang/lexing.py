from enum import Enum
import re
from typing import Sequence


class TokenType(Enum):
    NUMBER = "[+-]?([0-9]+[.])?[0-9]+"
    PARENTHESIS_BEGIN = "\("
    PARENTHESIS_END = "\)"
    BRACKET_BEGIN = "\["
    BRACKET_END = "\]"
    EQUAL = "="
    COMMA = ","
    DOT = "\."
    IF = "if "
    THEN = "then "
    ELSE = "else "
    EACH = "each "
    FOR = "for "
    IN = "in "
    FROM = "from "
    WHERE = "where "
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


def lexer(code: str) -> Sequence[Token]:
    num_characters = len(code)
    index = 0
    tokens = []
    while index < num_characters:
        token = _match_token(code, index)
        tokens.append(token)
        index += len(token)
    return [token for token in tokens if token.has_meaning()]


def _match_token(code: str, index: int) -> Token:
    for token_type in TokenType:
        regex = re.compile(token_type.value)
        match = regex.match(code, index)
        if match:
            return Token(type=token_type, value=match.group(0))
    raise "No matching token"
