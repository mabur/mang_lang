#pragma once

#include "expression.h"

struct Storage {
    std::vector<DynamicExpression> dynamic_expressions;
    std::vector<TypedExpression> typed_expressions;
    std::vector<EvaluatedDictionary> evaluated_dictionaries;
    std::vector<Dictionary> dictionaries;
    std::vector<Conditional> conditionals;
    std::vector<IsExpression> is_expressions;
    std::vector<Alternative> alternatives;
    std::vector<Function> functions;
    std::vector<FunctionBuiltIn> built_in_functions;
    std::vector<FunctionDictionary> dictionary_functions;
    std::vector<FunctionTuple> tuple_functions;
    std::vector<Tuple> tuples;
    std::vector<EvaluatedTuple> evaluated_tuples;
    std::vector<Stack> stacks;
    std::vector<EvaluatedStack> evaluated_stacks;
    std::vector<Table> tables;
    std::vector<EvaluatedTable> evaluated_tables;
    std::vector<EvaluatedTableView> evaluated_table_views;
    std::vector<LookupChild> child_lookups;
    std::vector<FunctionApplication> function_applications;
    std::vector<LookupSymbol> symbol_lookups;
    std::vector<Name> names;
    std::unordered_map<Name, size_t> name_indices;
    std::vector<Argument> arguments;
    std::vector<Number> numbers;
    std::vector<WhileStatement> while_statements;
    std::vector<ForStatement> for_statements;
    std::vector<ForSimpleStatement> for_simple_statements;
    std::vector<WhileEndStatement> while_end_statements;
    std::vector<ForEndStatement> for_end_statements;
    std::vector<Definition> definitions;
    std::vector<PutAssignment> put_assignments;
    std::vector<PutEachAssignment> put_each_assignments;
    std::vector<DropAssignment> drop_assignments;
    std::vector<String> strings;
};

extern Storage storage;

void clearMemory();

Character getCharacter(Expression expression);

Expression makeNumber(CodeRange code, Number expression);
Expression makeCharacter(CodeRange code, Character expression);
Expression makeDynamicExpression(CodeRange code, DynamicExpression expression);
Expression makeTypedExpression(CodeRange code, TypedExpression expression);
Expression makeConditional(CodeRange code, Conditional expression);
Expression makeIs(CodeRange code, IsExpression expression);
Expression makeAlternative(CodeRange code, Alternative expression);
Expression makeDictionary(CodeRange code, Dictionary expression);
Expression makeEvaluatedDictionary(CodeRange code, EvaluatedDictionary expression);
Expression makeFunction(CodeRange code, Function expression);
Expression makeFunctionBuiltIn(CodeRange code, FunctionBuiltIn expression);
Expression makeFunctionDictionary(CodeRange code, FunctionDictionary expression);
Expression makeFunctionTuple(CodeRange code, FunctionTuple expression);
Expression makeTuple(CodeRange code, Tuple expression);
Expression makeEvaluatedTuple(CodeRange code, EvaluatedTuple expression);
Expression makeStack(CodeRange code, Stack expression);
Expression makeEvaluatedStack(CodeRange code, EvaluatedStack expression);
Expression makeTable(CodeRange code, Table expression);
Expression makeEvaluatedTable(CodeRange code, EvaluatedTable expression);
Expression makeEvaluatedTableView(CodeRange code, EvaluatedTableView expression);
Expression makeLookupChild(CodeRange code, LookupChild expression);
Expression makeFunctionApplication(CodeRange code, FunctionApplication expression);
Expression makeLookupSymbol(CodeRange code, LookupSymbol expression);
Expression makeName(CodeRange code, Name expression);
Expression makeArgument(CodeRange code, Argument expression);
Expression makeDefinition(CodeRange code, Definition expression);
Expression makePutAssignment(CodeRange code, PutAssignment expression);
Expression makePutEachAssignment(CodeRange code, PutEachAssignment expression);
Expression makeDropAssignment(CodeRange code, DropAssignment expression);
Expression makeWhileStatement(CodeRange code, WhileStatement expression);
Expression makeForStatement(CodeRange code, ForStatement expression);
Expression makeForSimpleStatement(CodeRange code, ForSimpleStatement expression);
Expression makeWhileEndStatement(CodeRange code, WhileEndStatement expression);
Expression makeForEndStatement(CodeRange code, ForEndStatement expression);
Expression makeString(CodeRange code, String expression);
