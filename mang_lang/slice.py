class Slice:
    def __init__(self, elements: str, begin_index=0):
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

    def startswith(self, word: str) -> bool:
        return self._elements.startswith(word, self._begin_index)
