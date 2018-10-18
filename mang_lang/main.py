from interpreter import interpret, create_abstract_syntax_tree_json

code = '''
(
f(x) = mul(x[0], x[1]),
result = f(8, f(2, 3))
)
'''
print('Interpreting: {}'.format(code))
print('Abstract syntax tree:')
ast = create_abstract_syntax_tree_json(code)
print(ast)
value = interpret(code)
print('Value: {}'.format(value))
