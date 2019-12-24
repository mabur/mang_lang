from enum import Enum
from typing import Optional, Sequence

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

class FixedPraser:
    def __init__(self, token_type: TokenType):
        self.token_type = token_type

    def __call__(self, code: str, index: int) -> Optional[Token]:
        if code.startswith(self.token_type.value, index):
            return Token(type=self.token_type, value=code[:len(self.token_type.value)])
        return None

def parse_string(code: str, index: int) -> Optional[TokenType]:
    if code[index] != '\"':
        return None
    try:
        second = code.index('\"', index + 1)
        value = code[index:second + 1]
        return Token(type=TokenType.STRING, value=value)
    except ValueError:
        return None

def parse_number(code: str, index: int) -> Optional[TokenType]:
    end = index
    while end < len(code) and code[end] in '-+.1234567890':
        end += 1
    if end == index:
        return None
    return Token(type=TokenType.NUMBER, value=code[index:end])

def parse_symbol(code: str, index: int) -> Optional[TokenType]:
    end = index
    while end < len(code) and (code[end].isalnum() or code[end] == '_'):
        end += 1
    if end == index:
        return None
    return Token(type=TokenType.SYMBOL, value=code[index:end])

PARSERS = [
    parse_number,
    FixedPraser(TokenType.ARRAY_BEGIN),
    FixedPraser(TokenType.ARRAY_END),
    FixedPraser(TokenType.DICTIONARY_BEGIN),
    FixedPraser(TokenType.DICTIONARY_END),
    FixedPraser(TokenType.EQUAL),
    FixedPraser(TokenType.COMMA),
    FixedPraser(TokenType.IF),
    FixedPraser(TokenType.THEN),
    FixedPraser(TokenType.ELSE),
    FixedPraser(TokenType.EACH),
    FixedPraser(TokenType.FOR),
    FixedPraser(TokenType.IN),
    FixedPraser(TokenType.FROM),
    FixedPraser(TokenType.TO),
    FixedPraser(TokenType.OF),
    parse_symbol,
    parse_string,
    FixedPraser(TokenType.WHITE_SPACE),
    FixedPraser(TokenType.NEW_LINES),
]

def lexer(code: str) -> Sequence[Token]:
    code = code.replace('\n', ' ')
    num_characters = len(code)
    index = 0
    tokens = []
    while index < num_characters:
        token = _match_token(code, index)
        tokens.append(token)
        index += len(token)
    return [token for token in tokens if token.has_meaning()]


def _match_token(code: str, index: int) -> Token:
    for parser in PARSERS:
        token = parser(code, index)
        if token is not None:
            return token
    raise "No matching token"
