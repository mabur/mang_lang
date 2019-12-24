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

    def __call__(self, code: str, index: int) -> Token:
        return Token(type=self.token_type, value=code[:len(self.token_type.value)])

def parse_string(code: str, index: int) -> Token:
    assert code[index] == '\"'
    second = code.index('\"', index + 1)
    value = code[index:second + 1]
    return Token(type=TokenType.STRING, value=value)

def parse_number(code: str, index: int) -> Token:
    end = index
    while end < len(code) and code[end] in '-+.1234567890':
        end += 1
    assert end != index
    return Token(type=TokenType.NUMBER, value=code[index:end])

def parse_symbol(code: str, index: int) -> Token:
    end = index
    while end < len(code) and (code[end].isalnum() or code[end] == '_'):
        end += 1
    assert end != index
    return Token(type=TokenType.SYMBOL, value=code[index:end])

parser_from_token = {
    "[": FixedPraser(TokenType.ARRAY_BEGIN),
    "]": FixedPraser(TokenType.ARRAY_END),
    "{": FixedPraser(TokenType.DICTIONARY_BEGIN),
    "}": FixedPraser(TokenType.DICTIONARY_END),
    "=": FixedPraser(TokenType.EQUAL),
    ",": FixedPraser(TokenType.COMMA),
    "if ": FixedPraser(TokenType.IF),
    "then ": FixedPraser(TokenType.THEN),
    "else ": FixedPraser(TokenType.ELSE),
    "each ": FixedPraser(TokenType.EACH),
    "for ": FixedPraser(TokenType.FOR),
    "in ": FixedPraser(TokenType.IN),
    "from ": FixedPraser(TokenType.FROM),
    "to ": FixedPraser(TokenType.TO),
    "of ": FixedPraser(TokenType.OF),
    "\"": parse_string,
    " ": FixedPraser(TokenType.WHITE_SPACE),
    "\n": FixedPraser(TokenType.NEW_LINES),
}

for char in '+-.1234567890':
    parser_from_token[char] = parse_number

for char in 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_':
    parser_from_token[char] = parse_symbol

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
    for sequence, parser in parser_from_token.items():
        if code.startswith(sequence, index):
            return parser(code, index)
    raise "No matching token"
