from itertools import accumulate


class CodeFragment:
    """Describes a fragment of the source code starting at the given index"""
    def __init__(self, text: str, start_index=0):
        self.text = text
        self.start_index = start_index

    def __getitem__(self, item):
        return self.text[self.start_index + item]

    def __len__(self):
        return len(self.text) - self.start_index

    def pop(self):
        value = self[0]
        self.start_index += 1
        return value

    def front(self):
        return self[0]

    def startswith(self, word: str) -> bool:
        return self.text.startswith(word, self.start_index)


def print_syntax_error(text: CodeFragment) -> None:
    print('SYNTAX ERROR')
    print_error_description(error_label='SYNTAX ERROR', code=text)


def print_error_description(error_label: str, code: CodeFragment) -> None:
    """Print the source code that caused an error with a pointer to the error location"""
    lines = code.text.split('\n')
    cumulative_lengths = list(accumulate(len(line) + 1 for line in lines)) + [0]
    for row_number, line in enumerate(lines):
        length0 = cumulative_lengths[row_number - 1]
        length1 = cumulative_lengths[row_number]
        is_start_of_failure = length0 < code.start_index <= length1
        marker = '  <- {}'.format(error_label) if is_start_of_failure else ''
        print('{:04d} {}{}'.format(row_number, line, marker))
