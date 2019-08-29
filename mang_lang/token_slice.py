from typing import Sequence

from lexing import Token, TokenType


class TokenSlice:
    def __init__(self, tokens: Sequence[Token]):
        self._tokens = tokens
        self._begin_index = 0

    def front(self) -> str:
        return self._tokens[self._begin_index].value

    def do_match(self, token_pattern: Sequence[TokenType]) -> bool:
        if len(self._tokens) < self._begin_index + len(token_pattern):
            return False
        return all(
            self._tokens[self._begin_index + i].type == token for i, token in
            enumerate(token_pattern))

    def parse_token(self) -> str:
        value = self.front()
        self._begin_index += 1
        return value

    def parse_known_token(self, expected: TokenType) -> None:
        assert self.parse_token() == expected.value.replace('\\', '')
