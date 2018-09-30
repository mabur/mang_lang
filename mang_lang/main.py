import evaluating

code = 'add(5, mul(8, 9))'
print('Interpreting: {}'.format(code))
value = evaluating.interpret(code)
print('Value: {}'.format(value))
