from interpreter import interpret

code = 'add(5, mul(8, 9))'
print('Interpreting: {}'.format(code))
value = interpret(code)
print('Value: {}'.format(value))
