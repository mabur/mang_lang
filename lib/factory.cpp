#include "factory.h"

#include <cstring>

#include <carma/carma.h>

#include "exceptions.h"
#include "mang_lang_string.h"

Storage storage;

template<typename ElementType, typename ArrayType>
static
Expression makeExpression(
    CodeRange code,
    ElementType&& expression,
    ExpressionType type,
    ArrayType& array
) {
    APPEND(array, expression);
    return Expression{array.count - 1, code, type};
}

void clearMemory() {
    FREE_DARRAY(storage.code_characters);
    FREE_DARRAY(storage.code_rows);
    FREE_DARRAY(storage.code_columns);
    
    FREE_DARRAY(storage.dynamic_expressions);
    FREE_DARRAY(storage.typed_expressions);
    FREE_DARRAY(storage.dictionary_expressions);
    FREE_DARRAY(storage.dictionary_values);
    FREE_DARRAY(storage.conditional_expressions);
    FREE_DARRAY(storage.is_expressions);
    FREE_DARRAY(storage.alternatives);
    FREE_DARRAY(storage.function_expressions);
    FREE_DARRAY(storage.function_built_in_values);
    FREE_DARRAY(storage.function_dictionary_expressions);
    FREE_DARRAY(storage.function_tuple_expressions);
    FREE_DARRAY(storage.function_values);
    FREE_DARRAY(storage.tuple_expressions);
    FREE_DARRAY(storage.tuple_values);
    FREE_DARRAY(storage.stack_expressions);
    FREE_DARRAY(storage.stack_values);
    FREE_DARRAY(storage.table_view_values);
    FREE_DARRAY(storage.lookup_child_expressions);
    FREE_DARRAY(storage.function_application_expressions);
    FREE_DARRAY(storage.function_application_built_in_expressions);
    FREE_DARRAY(storage.lookup_symbol_expressions);
    FREE_DARRAY(storage.argument_types);
    FREE_DARRAY(storage.while_statements);
    FREE_DARRAY(storage.for_init_statements);
    FREE_DARRAY(storage.for_statements);
    FREE_DARRAY(storage.if_statements);
    FREE_DARRAY(storage.while_end_statements);
    FREE_DARRAY(storage.for_end_statements);
    FREE_DARRAY(storage.for_iterators);
    FREE_DARRAY(storage.definition_statements);
    FREE_DARRAY(storage.put_assignment_statements);
    FREE_DARRAY(storage.put_each_assignment_statements);
    FREE_DARRAY(storage.drop_assignment_statements);
    FREE_DARRAY(storage.statements);
    FREE_DARRAY(storage.expressions);
    FREE_DARRAY(storage.slot_names);
    FREE_DARRAY(storage.slot_values);
    FREE_DARRAY(storage.strings);
    FREE_DARRAY(storage.rows);
    FREE_DARRAY(storage.table_expressions);
    FREE_DARRAY(storage.names);

    FREE_DARRAY(storage.name_index_table);
    
    storage.table_values.clear();
}

// MAKERS:

#define BIT_CAST(source, target) do { \
    static_assert(sizeof(source) == sizeof(target), "BIT_CAST"); \
    memcpy(&target, &source, sizeof(source)); \
} while (0)

Expression makeNumber(CodeRange code, Number expression) {
    auto result = Expression{};
    result.type = NUMBER;
    result.range = code;
    BIT_CAST(expression, result.index);
    return result;
}

Expression makeErrorValue(CodeRange code, const char* format, ...) {
    va_list args;
    va_start(args, format);
    ErrorValue expression = format_cstring_v(format, args);
    va_end(args);
    auto result = Expression{};
    result.type = ERROR_VALUE;
    result.range = code;
    BIT_CAST(expression, result.index);
    return result;
}

Expression makeCharacter(CodeRange code, Character expression) {
    return Expression{static_cast<size_t>(expression), code, CHARACTER};
}

Expression makeDynamicExpression(CodeRange code, DynamicExpression expression) {
    return makeExpression(code, expression, DYNAMIC_EXPRESSION, storage.dynamic_expressions);
}

Expression makeTypedExpression(CodeRange code, TypedExpression expression) {
    return makeExpression(code, expression, TYPED_EXPRESSION, storage.typed_expressions);
}

Expression makeConditionalExpression(CodeRange code, ConditionalExpression expression) {
    return makeExpression(code, expression, CONDITIONAL_EXPRESSION, storage.conditional_expressions);
}

Expression makeIsExpression(CodeRange code, IsExpression expression) {
    return makeExpression(code, expression, IS_EXPRESSION, storage.is_expressions);
}

Expression makeAlternative(CodeRange code, Alternative expression) {
    return makeExpression(code, expression, ALTERNATIVE, storage.alternatives);
}

Expression makeDictionaryExpression(CodeRange code, DictionaryExpression expression) {
    return makeExpression(code, expression, DICTIONARY_EXPRESSION, storage.dictionary_expressions);
}

Expression makeDictionaryValue(CodeRange code, DictionaryValue expression) {
    return makeExpression(code, expression, DICTIONARY_VALUE, storage.dictionary_values);
}

Expression makeFunctionExpression(CodeRange code, FunctionExpression expression) {
    return makeExpression(code, expression, FUNCTION_EXPRESSION, storage.function_expressions);
}

Expression makeFunctionBuiltInValue(CodeRange code, FunctionBuiltInValue expression) {
    return makeExpression(code, expression, FUNCTION_BUILT_IN_VALUE, storage.function_built_in_values);
}

Expression makeFunctionDictionaryExpression(CodeRange code, FunctionDictionaryExpression expression) {
    return makeExpression(code, expression, FUNCTION_DICTIONARY_EXPRESSION, storage.function_dictionary_expressions);
}

Expression makeFunctionTupleExpression(CodeRange code, FunctionTupleExpression expression) {
    return makeExpression(code, expression, FUNCTION_TUPLE_EXPRESSION, storage.function_tuple_expressions);
}

Expression makeFunctionValue(CodeRange code, FunctionValue expression) {
    return makeExpression(code, expression, FUNCTION_VALUE, storage.function_values);
}

Expression makeTupleExpression(CodeRange code, TupleExpression expression) {
    return makeExpression(code, expression, TUPLE_EXPRESSION, storage.tuple_expressions);
}

Expression makeTupleValue(CodeRange code, TupleValue expression) {
    return makeExpression(code, expression, TUPLE_VALUE, storage.tuple_values);
}

Expression makeTupleValue2(Expression a, Expression b) {
    const auto first = storage.expressions.count;
    APPEND(storage.expressions, a);
    APPEND(storage.expressions, b);
    const auto last = storage.expressions.count;
    return makeTupleValue(CodeRange{}, TupleValue{Indices{first, last - first}});
}

Expression makeStackExpression(CodeRange code, StackExpression expression) {
    return makeExpression(code, expression, STACK_EXPRESSION, storage.stack_expressions);
}

Expression makeStackValue(CodeRange code, StackValue expression) {
    return makeExpression(code, expression, STACK_VALUE, storage.stack_values);
}

Expression makeTableExpression(CodeRange code, TableExpression expression) {
    return makeExpression(code, expression, TABLE_EXPRESSION, storage.table_expressions);
}

Expression makeTableValue(CodeRange code, TableValue expression) {
    storage.table_values.emplace_back(std::move(expression));
    return Expression{storage.table_values.size() - 1, code, TABLE_VALUE};
}

Expression makeTableViewValue(CodeRange code, TableViewValue expression) {
    return makeExpression(code, expression, TABLE_VIEW_VALUE, storage.table_view_values);
}

Expression makeLookupChildExpression(CodeRange code, LookupChildExpression expression) {
    return makeExpression(code, expression, LOOKUP_CHILD_EXPRESSION, storage.lookup_child_expressions);
}

Expression makeFunctionApplicationExpression(CodeRange code, FunctionApplicationExpression expression) {
    return makeExpression(code, expression, FUNCTION_APPLICATION_EXPRESSION, storage.function_application_expressions);
}

Expression makeFunctionApplicationBuiltInExpression(CodeRange code, FunctionApplicationBuiltInExpression expression) {
    return makeExpression(code, expression, FUNCTION_APPLICATION_BUILT_IN_EXPRESSION, storage.function_application_built_in_expressions);
}

Expression makeLookupSymbolExpression(CodeRange code, LookupSymbolExpression expression) {
    return makeExpression(code, expression, LOOKUP_SYMBOL_EXPRESSION, storage.lookup_symbol_expressions);
}

Expression makeName(CodeRange code, const char* data, size_t count) {
    auto string = StringView{data, count};
    FOR_EACH(item, storage.name_index_table) {
        if (ARE_EQUAL(item->key, string)) {
            return Expression{item->value, code, NAME};
        }
    }
    auto index = storage.names.count;
    APPEND(storage.name_index_table, (NameIndex{string, index}));
    CONCAT(storage.names, string);
    APPEND(storage.names, '\0');
    return Expression{index, code, NAME};
}

Expression makeDefinitionStatement(CodeRange code, DefinitionStatement expression) {
    return makeExpression(code, expression, DEFINITION_STATEMENT, storage.definition_statements);
}

Expression makePutAssignmentStatement(CodeRange code, PutAssignmentStatement expression) {
    return makeExpression(code, expression, PUT_ASSIGNMENT_STATEMENT, storage.put_assignment_statements);
}

Expression makePutEachAssignmentStatement(CodeRange code, PutEachAssignmentStatement expression) {
    return makeExpression(code, expression, PUT_EACH_ASSIGNMENT_STATEMENT, storage.put_each_assignment_statements);
}

Expression makeDropAssignmentStatement(CodeRange code, DropAssignmentStatement expression) {
    return makeExpression(code, expression, DROP_ASSIGNMENT_STATEMENT, storage.drop_assignment_statements);
}

Expression makeWhileStatement(CodeRange code, WhileStatement expression) {
    return makeExpression(code, expression, WHILE_STATEMENT, storage.while_statements);
}

Expression makeForInitStatement(CodeRange code, ForInitStatement expression) {
    return makeExpression(code, expression, FOR_INIT_STATEMENT, storage.for_init_statements);
}

Expression makeForStatement(CodeRange code, ForStatement expression) {
    return makeExpression(code, expression, FOR_STATEMENT, storage.for_statements);
}

Expression makeIfStatement(CodeRange code, IfStatement expression) {
    return makeExpression(code, expression, IF_STATEMENT, storage.if_statements);
}

Expression makeWhileEndStatement(CodeRange code, WhileEndStatement expression) {
    return makeExpression(code, expression, WHILE_END_STATEMENT, storage.while_end_statements);
}

Expression makeForEndStatement(CodeRange code, ForEndStatement expression) {
    return makeExpression(code, expression, FOR_END_STATEMENT, storage.for_end_statements);
}

Expression makeForIterator(CodeRange code, ForIterator expression) {
    return makeExpression(code, expression, FOR_ITERATOR, storage.for_iterators);
}

Expression makeString(CodeRange code, String expression) {
    return makeExpression(code, expression, STRING, storage.strings);
}

// GETTERS

Character getCharacter(Expression expression) {
    CHECK_INTERNAL(expression.index <= 127, 
        "I found an internal error while retrieving a character.\n"
        "A character should have an ASCII value in the range 0-127.\n"
        "But I found one with the ASCII value %zu.",
        expression.index
    );
    return (Character)expression.index;
}

Number getNumber(Expression expression) {
    Number result;
    BIT_CAST(expression.index, result);
    return result;
}

ErrorValue getErrorValue(Expression expression) {
    ErrorValue result;
    BIT_CAST(expression.index, result);
    return result;
}

CodeRange makeCodeCharacters(const char* s) {
    auto string = STRING_VIEW(s);
    auto result = CodeRange{
        CharacterIndex(storage.code_characters.count),
        CharacterIndex(string.count)
    };
    auto column = CharacterIndex{1};
    auto row = CharacterIndex{1};
    FOR_EACH(character, string) {
        APPEND(storage.code_characters, *character);
        APPEND(storage.code_rows, row);
        APPEND(storage.code_columns, column);
        ++column;
        if (*character == '\n') {
            ++row;
            column = 1;
        }
    }
    // Sentinel entry so an empty CodeRange right after the last character
    // still has a valid row/column to report.
    // This happens can happen if the input stops to early.
    APPEND(storage.code_characters, '\0');
    APPEND(storage.code_rows, row);
    APPEND(storage.code_columns, column);
    return result;
}

char firstCharacter(CodeRange code) {
    return storage.code_characters.data[code.data];
}

size_t firstColumn(CodeRange code) {
    return storage.code_columns.data[code.data];
}

size_t firstRow(CodeRange code) {
    return storage.code_rows.data[code.data];
}

char lastCharacter(CodeRange code) {
    return storage.code_characters.data[code.data + code.count - 1];
}

size_t lastColumn(CodeRange code) {
    return storage.code_columns.data[code.data + code.count - 1];
}

size_t lastRow(CodeRange code) {
    return storage.code_rows.data[code.data + code.count - 1];
}
