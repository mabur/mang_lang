from typing import Sequence

class Slice:
    def __init__(self, elements: Sequence):
        self._elements = elements
        self._begin_index = 0

    def __getitem__(self, item):
        return self._elements[self._begin_index + item]

    def __len__(self):
        return len(self._elements) - self._begin_index

    def pop(self):
        value = self[0]
        self._begin_index += 1
        return value
