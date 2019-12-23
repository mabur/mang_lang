from typing import Sequence

from lexing import TokenType
from slice import Slice


class TokenSlice(Slice):
    def do_match(self, token_pattern: TokenType) -> bool:
        return len(self) and self[0].type == token_pattern

    def parse_known_token(self, expected: TokenType) -> None:
        actual = self.pop()
        assert expected == actual.type, 'Expected {} but got {}'.format(expected, actual.type)
        return actual.value
