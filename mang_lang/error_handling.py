from itertools import accumulate


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


class Section:
    def __init__(self, text: Slice):
        self.text = text
        self._begin_index = text._begin_index

    @property
    def _elements(self):
        return self.text._elements


def print_syntax_error(text: Slice) -> None:
    print('SYNTAX ERROR')
    _print_error_description(error_label='SYNTAX ERROR', code=text)


class AlreadyRegisteredException(Exception):
    pass


def run_time_error_printer(evaluate):
    def wrapped_evaluate(self, environment):
        try:
            return evaluate(self, environment)
        except AlreadyRegisteredException:
            pass
        except:
            print('Run time error when evaluating {}:'.format(self.__class__.__name__))
            _print_error_description(error_label='RUN TIME ERROR', code=self.section)
            raise AlreadyRegisteredException
    return wrapped_evaluate


def _print_error_description(error_label: str, code: Slice) -> None:
    '''Print the source code that caused an error with a pointer to the error location'''
    lines = code._elements.split()
    cumulative_lengths = list(accumulate(len(line) + 1 for line in lines)) + [0]
    for row_number, line in enumerate(lines):
        length0 = cumulative_lengths[row_number - 1]
        length1 = cumulative_lengths[row_number]
        is_start_of_failure = length0 < code._begin_index <= length1
        marker = '  <- {}'.format(error_label) if is_start_of_failure else ''
        print('{:04d} {}{}'.format(row_number, line, marker))
