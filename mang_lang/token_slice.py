from typing import Sequence

from lexing import TokenType
from slice import Slice


class TokenSlice(Slice):
    def front(self) -> str:
        return self[0].value

    def do_match(self, token_pattern: Sequence[TokenType]) -> bool:
        if len(self) < len(token_pattern):
            return False
        return all(
            self[i].type == token for i, token in
            enumerate(token_pattern))

    def parse_token(self) -> str:
        return self.pop().value

    def parse_known_token(self, expected: TokenType) -> None:
        assert self.parse_token() == expected.value.replace('\\', '')
