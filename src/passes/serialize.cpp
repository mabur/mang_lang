#include "serialize.h"

#include <limits>
#include <sstream>

#include <carma/carma.h>

#include "../factory.h"
#include "../string.h"

namespace {

DynamicString serializeName(DynamicString s, size_t name) {
    s = concatcstring(s, storage.names.at(name).c_str());
    return s;
}

DynamicString serializeArgument(DynamicString s, Argument a) {
    if (a.type.type != ANY) {
        s = serialize(s, a.type);
        s = concatcstring(s, ":");
        s = serializeName(s, a.name);
        return s;
    }
    s = serializeName(s, a.name);
    return s;
}

DynamicString serializeDynamicExpression(DynamicString s, const DynamicExpression& dynamic_expression) {
    s = concatcstring(s, "dynamic ");
    s = serialize(s, dynamic_expression.expression);
    return s;
}

DynamicString serializeTypedExpression(DynamicString s, const TypedExpression& typed_expression) {
    s = serializeName(s, typed_expression.type_name.global_index);
    s = concatcstring(s, ":");
    s = serialize(s, typed_expression.value);
    return s;
}

DynamicString serializeConditional(DynamicString s, const Conditional& conditional) {
    s = concatcstring(s, "if ");
    for (auto a = conditional.alternative_first;
        a.index <= conditional.alternative_last.index;
        ++a.index
    ) {
        const auto alternative = storage.alternatives.data[a.index];
        s = serialize(s, alternative.left);
        s = concatcstring(s, " then ");
        s = serialize(s, alternative.right);
        s = concatcstring(s, " ");
    }
    s = concatcstring(s, "else ");
    s = serialize(s, conditional.expression_else);
    return s;
}

DynamicString serializeIs(DynamicString s, const IsExpression& is_expression) {
    s = concatcstring(s, "is ");
    s = serialize(s, is_expression.input);
    s = concatcstring(s, " ");
    for (auto a = is_expression.alternative_first;
        a.index <= is_expression.alternative_last.index;
        ++a.index
    ) {
        const auto alternative = storage.alternatives.data[a.index];
        s = serialize(s, alternative.left);
        s = concatcstring(s, " then ");
        s = serialize(s, alternative.right);
        s = concatcstring(s, " ");
    }
    s = concatcstring(s, "else ");
    s = serialize(s, is_expression.expression_else);
    return s;
}

DynamicString serializeDefinition(DynamicString s, const Definition& element) {
    s = serializeName(s, element.name.global_index);
    s = concatcstring(s, "=");
    s = serialize(s, element.expression);
    s = concatcstring(s, " ");
    return s;
}

DynamicString serializePutAssignment(DynamicString s, const PutAssignment& element) {
    s = serializeName(s, element.name.global_index);
    s = concatcstring(s, "+=");
    s = serialize(s, element.expression);
    s = concatcstring(s, " ");
    return s;
}

DynamicString serializePutEachAssignment(DynamicString s, const PutEachAssignment& element) {
    s = serializeName(s, element.name.global_index);
    s = concatcstring(s, "++=");
    s = serialize(s, element.expression);
    s = concatcstring(s, " ");
    return s;
}

DynamicString serializeDropAssignment(DynamicString s, const DropAssignment& element) {
    s = serializeName(s, element.name.global_index);
    s = concatcstring(s, "-- ");
    return s;
}

DynamicString serializeWhileStatement(DynamicString s, const WhileStatement& element) {
    s = concatcstring(s, "while ");
    s = serialize(s, element.expression);
    s = concatcstring(s, " ");
    return s;
}

DynamicString serializeForStatement(DynamicString s, const ForStatement& element) {
    s = concatcstring(s, "for ");
    s = serializeName(s, element.item_name.global_index);
    s = concatcstring(s, " in ");
    s = serializeName(s, element.container_name.global_index);
    s = concatcstring(s, " ");
    return s;
}

DynamicString serializeForSimpleStatement(DynamicString s, const ForSimpleStatement& element) {
    s = concatcstring(s, "for ");
    s = serializeName(s, element.container_name.global_index);
    s = concatcstring(s, " ");
    return s;
}

template<typename Serializer>
DynamicString serializeEvaluatedDictionary(DynamicString s, Serializer serializer, const EvaluatedDictionary& dictionary) {
    if (dictionary.definitions.empty()) {
        s = concatcstring(s, "{}");
        return s;
    }
    s = concatcstring(s, "{");
    for (const auto& pair : dictionary.definitions) {
        s = serializeName(s, pair.name.global_index);
        s = concatcstring(s, "=");
        s = serializer(s, pair.expression);
        s = concatcstring(s, " ");
    }
    LAST_ITEM(s) = '}';
    return s;
}

template<typename Serializer>
DynamicString serializeEvaluatedTuple(DynamicString s, Serializer serializer, Expression t) {
    const auto evaluated_tuple = storage.evaluated_tuples.data[t.index];
    if (evaluated_tuple.first == evaluated_tuple.last) {
        s = concatcstring(s, "()");
        return s;
    }
    s = concatcstring(s, "(");
    for (size_t i = evaluated_tuple.first; i < evaluated_tuple.last; ++i) {
        const auto expression = storage.expressions.data[i];
        s = serializer(s, expression);
        s = concatcstring(s, " ");
    }
    LAST_ITEM(s) = ')';
    return s;
}

DynamicString serializeLookupChild(DynamicString s, const LookupChild& lookup_child) {
    s = serializeName(s, lookup_child.name);
    s = concatcstring(s, "@");
    s = serialize(s, lookup_child.child);
    return s;
}

DynamicString serializeFunctionApplication(DynamicString s, const FunctionApplication& function_application) {
    s = serializeName(s, function_application.name.global_index);
    s = concatcstring(s, "!");
    s = serialize(s, function_application.child);
    return s;
}

DynamicString serializeLookupSymbol(DynamicString s, const LookupSymbol& lookup_symbol) {
    s = serializeName(s, lookup_symbol.name.global_index);
    return s;
}
    
DynamicString serializeDictionary(DynamicString s, const Dictionary& dictionary) {
    s = concatcstring(s, "{");
    for (size_t i = dictionary.statement_first; i < dictionary.statement_last; ++i) {
        const auto statement = storage.statements.data[i];
        s = serialize(s, statement);
    }
    // TODO: handle by early return.
    if (dictionary.statement_first == dictionary.statement_last) {
        s = concatcstring(s, "}");
    }
    else {
        LAST_ITEM(s) = '}';
    }
    return s;
}

DynamicString serializeTuple(DynamicString s, Expression t) {
    const auto tuple_struct = storage.tuples.data[t.index];
    if (tuple_struct.first == tuple_struct.last) {
        s = concatcstring(s, "()");
        return s;
    }
    s = concatcstring(s, "(");
    for (size_t i = tuple_struct.first; i < tuple_struct.last; ++i) {
        const auto expression = storage.expressions.data[i];
        s = serialize(s, expression);
        s = concatcstring(s, " ");
    }
    LAST_ITEM(s) = ')';
    return s;
}

DynamicString serializeStack(DynamicString s, Expression expression) {
    s = concatcstring(s, "[");
    while (expression.type != EMPTY_STACK) {
        if (expression.type != STACK) {
            throw std::runtime_error(
                std::string{"\n\nI have found a type error."} +
                    "\nIt happens in serializeStack. " +
                    "\nInstead of a stack I got a " + NAMES[expression.type] +
                    ".\n"
            );
        }
        const auto stack = storage.stacks.data[expression.index];
        s = serialize(s, stack.top);
        s = concatcstring(s, " ");
        expression = stack.rest;
    }
    LAST_ITEM(s) = ']';
    return s;
}

DynamicString serializeCharacter(DynamicString s, Character character) {
    APPEND(s, '\'');
    APPEND(s, character);
    APPEND(s, '\'');
    return s;
}

DynamicString serializeFunction(DynamicString s, const Function& function) {
    s = concatcstring(s, "in ");
    s = serializeArgument(s, storage.arguments.data[function.argument]);
    s = concatcstring(s, " out ");
    s = serialize(s, function.body);
    return s;
}

DynamicString serializeFunctionDictionary(DynamicString s, const FunctionDictionary& function_dictionary) {
    s = concatcstring(s, "in ");
    s = concatcstring(s, "{");
    for (auto i = function_dictionary.first_argument; i < function_dictionary.last_argument; ++i) {
        s = serializeArgument(s, storage.arguments.data[i]);
        s = concatcstring(s, " ");
    }
    if (function_dictionary.first_argument == function_dictionary.last_argument) {
        s = concatcstring(s, "}");
    }
    else {
        LAST_ITEM(s) = '}';
    }
    s = concatcstring(s, " out ");
    s = serialize(s, function_dictionary.body);
    return s;
}

DynamicString serializeFunctionTuple(DynamicString s, const FunctionTuple& function_stack) {
    s = concatcstring(s, "in ");
    s = concatcstring(s, "(");
    for (auto i = function_stack.first_argument; i < function_stack.last_argument; ++i) {
        s = serializeArgument(s, storage.arguments.data[i]);
        s = concatcstring(s, " ");
    }
    if (function_stack.first_argument == function_stack.last_argument) {
        s = concatcstring(s, ")");
    }
    else {
        LAST_ITEM(s) = ')';
    }
    s = concatcstring(s, " out ");
    s = serialize(s, function_stack.body);
    return s;
}
    
DynamicString serializeTable(DynamicString s, Expression t) {
    const auto rows = storage.tables.at(t.index).rows;
    if (rows.empty()) {
        return s = concatcstring(s, "<>");
    }
    s = concatcstring(s, "<");
    for (const auto& row : rows) {
        s = concatcstring(s, "(");
        s = serialize(s, row.key);
        s = concatcstring(s, " ");
        s = serialize(s, row.value);
        s = concatcstring(s, ") ");
    }
    LAST_ITEM(s) = '>';
    return s;
}

DynamicString serializeTypesEvaluatedTable(DynamicString s, Expression t) {
    const auto& rows = storage.evaluated_tables.at(t.index).rows;
    if (rows.empty()) {
        s = concatcstring(s, "<>");
        return s;
    }
    const auto& row = rows.begin()->second;
    s = concatcstring(s, "<(");
    s = serialize_types(s, row.key);
    s = concatcstring(s, " ");
    s = serialize_types(s, row.value);
    s = concatcstring(s, ")>");
    return s;
}

template<typename Table>
DynamicString serializeEvaluatedTable(DynamicString s, const Table& table) {
    if (table.empty()) {
        s = concatcstring(s, "<>");
        return s;
    }
    s = concatcstring(s, "<");
    for (const auto& row : table) {
        s = concatcstring(s, "(");
        s = concatcstring(s, row.first.c_str());
        s = concatcstring(s, " ");
        s = serialize(s, row.second.value);
        s = concatcstring(s, ") ");
    }
    LAST_ITEM(s) = '>';
    return s;
}

DynamicString serializeTypesEvaluatedStack(DynamicString s, Expression e) {
    s = concatcstring(s, "[");
    s = serialize_types(s, storage.evaluated_stacks.data[e.index].top);
    s = concatcstring(s, "]");
    return s;
}

DynamicString serializeEvaluatedStack(DynamicString s, Expression expression) {
    s = concatcstring(s, "[");
    while (expression.type != EMPTY_STACK) {
        if (expression.type != EVALUATED_STACK) {
            throw std::runtime_error{
                "I found an error while serializing a stack. "
                "Instead of a stack I got a " + NAMES[expression.type]
            };
        }
        const auto stack = storage.evaluated_stacks.data[expression.index];
        s = serialize(s, stack.top);
        s = concatcstring(s, " ");
        expression = stack.rest;
    }
    LAST_ITEM(s) = ']';
    return s;
}

DynamicString serializeNumber(DynamicString s, Number number) {
    if (number != number) {
        s = concatcstring(s, "nan");
        return s;
    }
    std::stringstream stream;
    stream.precision(std::numeric_limits<double>::digits10 + 1);
    stream << number;
    s = concatcstring(s, stream.str().c_str());
    return s;
}

DynamicString serializeString(DynamicString s, Expression expression) {
    s = concatcstring(s, "\"");
    while (expression.type != EMPTY_STRING) {
        if (expression.type != STRING) {
            throw std::runtime_error{
                "I found an error while serializing a string. "
                "Instead of a string I got a " + NAMES[expression.type]
            };
        }
        const auto string = storage.strings.data[expression.index];
        const auto top = string.top;
        const auto rest = string.rest;
        if (top.type != CHARACTER) {
            throw std::runtime_error{
                "I found an error while serializing a string. "
                "Each item in the string should be a character, "
                "but I found a " + NAMES[top.type]
            };
        }
        APPEND(s, getCharacter(top));
        expression = rest;
    }
    s = concatcstring(s, "\"");
    return s;
}

} // namespace

DynamicString serialize_types(DynamicString s, Expression expression) {
    switch (expression.type) {
        case EVALUATED_DICTIONARY: return serializeEvaluatedDictionary(s, serialize_types, storage.evaluated_dictionaries.at(expression.index));
        case EVALUATED_TUPLE: return serializeEvaluatedTuple(s, serialize_types, expression);
        case EVALUATED_STACK: return serializeTypesEvaluatedStack(s, expression);
        case EVALUATED_TABLE: return serializeTypesEvaluatedTable(s, expression);
        // TODO: EVALUATED_TABLE_VIEW?
        default: s = concatcstring(s, NAMES[expression.type].c_str()); return s;
    }
}

DynamicString serialize(DynamicString s, Expression expression) {
    switch (expression.type) {
        case CHARACTER: return serializeCharacter(s, getCharacter(expression));
        case CONDITIONAL: return serializeConditional(s, storage.conditionals.data[expression.index]);
        case IS: return serializeIs(s, storage.is_expressions.data[expression.index]);
        case DICTIONARY: return serializeDictionary(s, storage.dictionaries.data[expression.index]);
        case EVALUATED_DICTIONARY: return serializeEvaluatedDictionary(s, serialize, storage.evaluated_dictionaries.at(expression.index));
        case DEFINITION: return serializeDefinition(s, storage.definitions.data[expression.index]);
        case PUT_ASSIGNMENT: return serializePutAssignment(s, storage.put_assignments.data[expression.index]);
        case PUT_EACH_ASSIGNMENT: return serializePutEachAssignment(s, storage.put_each_assignments.data[expression.index]);
        case DROP_ASSIGNMENT: return serializeDropAssignment(s, storage.drop_assignments.data[expression.index]);
        case WHILE_STATEMENT: return serializeWhileStatement(s, storage.while_statements.data[expression.index]);
        case FOR_STATEMENT: return serializeForStatement(s, storage.for_statements.data[expression.index]);
        case FOR_SIMPLE_STATEMENT: return serializeForSimpleStatement(s, storage.for_simple_statements.data[expression.index]);
        case FUNCTION: return serializeFunction(s, storage.functions.data[expression.index]);
        case FUNCTION_DICTIONARY: return serializeFunctionDictionary(s, storage.dictionary_functions.data[expression.index]);
        case FUNCTION_TUPLE: return serializeFunctionTuple(s, storage.tuple_functions.data[expression.index]);
        case TABLE: return serializeTable(s, expression);
        case EVALUATED_TABLE: return serializeEvaluatedTable(s, storage.evaluated_tables.at(expression.index).rows);
        case EVALUATED_TABLE_VIEW: return serializeEvaluatedTable(s, storage.evaluated_table_views.data[expression.index]);
        case TUPLE: return serializeTuple(s, expression);
        case EVALUATED_TUPLE: return serializeEvaluatedTuple(s, serialize, expression);
        case STACK: return serializeStack(s, expression);
        case EVALUATED_STACK: return serializeEvaluatedStack(s, expression);
        case LOOKUP_CHILD: return serializeLookupChild(s, storage.child_lookups.data[expression.index]);
        case FUNCTION_APPLICATION: return serializeFunctionApplication(s, storage.function_applications.data[expression.index]);
        case LOOKUP_SYMBOL: return serializeLookupSymbol(s, storage.symbol_lookups.data[expression.index]);
        case NUMBER: return serializeNumber(s, getNumber(expression));
        case EMPTY_STRING: return serializeString(s, expression);
        case STRING: return serializeString(s, expression);
        case DYNAMIC_EXPRESSION: return serializeDynamicExpression(s, storage.dynamic_expressions.data[expression.index]);
        case TYPED_EXPRESSION: return serializeTypedExpression(s, storage.typed_expressions.data[expression.index]);
        case EMPTY_STACK: return s = concatcstring(s, "[]");
        case YES: return s = concatcstring(s, "yes");
        case NO: return s = concatcstring(s, "no");
        case WHILE_END_STATEMENT: return s = concatcstring(s, "end ");
        case FOR_END_STATEMENT: return s = concatcstring(s, "end ");
        case FOR_SIMPLE_END_STATEMENT: return s = concatcstring(s, "end ");
        case RETURN_STATEMENT: return s = concatcstring(s, "return ");
        default: return s = concatcstring(s, NAMES[expression.type].c_str());
    }
}
