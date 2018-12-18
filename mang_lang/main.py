from interpreter import interpret, create_abstract_syntax_tree_json
import json

code = '''
(f(x)=1, all f(y) for y in (1,2))
'''
print('Interpreting: {}'.format(code))
print('Abstract syntax tree:')
ast = create_abstract_syntax_tree_json(code)
print(json.dumps(ast, indent=1))
print('Interpreting: {}'.format(code))
value = interpret(code)
print('Value:')
print(json.dumps(value, indent=1))
