from typing import Sequence

from lexing import TokenType
from slice import Slice


class TokenSlice(Slice):
    def do_match(self, token_pattern: TokenType) -> bool:
        return len(self) and self[0].type == token_pattern

    def parse(self, expected: TokenType) -> str:
        actual = self.pop()
        assert expected == actual.type, 'Expected {} but got {}'.format(expected, actual.type)
        return actual.value
