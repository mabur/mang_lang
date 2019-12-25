from typing import Sequence

class Slice:
    def __init__(self, elements: Sequence, begin_index=0):
        self._elements = elements
        self._begin_index = begin_index

    def __getitem__(self, item):
        return self._elements[self._begin_index + item]

    def __len__(self):
        return len(self._elements) - self._begin_index

    def pop(self):
        value = self[0]
        self._begin_index += 1
        return value

    def front(self):
        return self[0]

    def startswith(self, sequence) -> bool:
        # TODO: Assuming list here instead of more general Sequence.
        return self._elements.startswith(sequence, self._begin_index)
