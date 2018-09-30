import lexing
import parsing

example_code = 'add(5, mul(8, 9))'
print('Code:')
print(example_code)
print('Lexing...')
tokens = lexing.lexer(example_code)
for token in tokens:
    print('{} {}'.format(token.value, token.type.name))
print('Done.')
print('Parsing:')
expression = parsing.parse_expression(tokens=tokens)
print('Done')
print(expression)
