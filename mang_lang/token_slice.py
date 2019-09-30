from typing import Sequence

from lexing import TokenType
from slice import Slice


class TokenSlice(Slice):
    def do_match(self, token_pattern: Sequence[TokenType]) -> bool:
        return len(token_pattern) <= len(self) and \
               all(a.type == b for a, b in zip(self, token_pattern))

    def parse_token(self) -> str:
        return self.pop().value

    def parse_known_token(self, expected: TokenType) -> None:
        actual = self.pop().type
        assert expected == actual, 'Expected {} but got {}'.format(expected, actual)
