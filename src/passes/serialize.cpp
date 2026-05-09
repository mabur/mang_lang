#include "serialize.h"

#include <float.h>

#include <carma/carma.h>

#include "../exceptions.h"
#include "../factory.h"
#include "../string.h"

namespace {

StringBuilder serializeName(StringBuilder s, size_t name) {
    s = concatenate(s, storage.names.data + name);
    return s;
}

StringBuilder serializeArgument(StringBuilder s, Argument a) {
    if (a.type.type != ANY) {
        s = serialize(s, a.type);
        s = concatenate(s, ":");
        s = serializeName(s, a.name);
        return s;
    }
    s = serializeName(s, a.name);
    return s;
}

StringBuilder serializeDynamicExpression(StringBuilder s, const DynamicExpression& dynamic_expression) {
    s = concatenate(s, "dynamic ");
    s = serialize(s, dynamic_expression.expression);
    return s;
}

StringBuilder serializeTypedExpression(StringBuilder s, const TypedExpression& typed_expression) {
    s = serializeName(s, typed_expression.type_name.global_index);
    s = concatenate(s, ":");
    s = serialize(s, typed_expression.value);
    return s;
}

StringBuilder serializeConditional(StringBuilder s, const Conditional& conditional) {
    s = concatenate(s, "if ");
    FOR_EACH(a, conditional.alternatives) {
        const auto alternative = storage.alternatives.data[a];
        s = serialize(s, alternative.left);
        s = concatenate(s, " then ");
        s = serialize(s, alternative.right);
        s = concatenate(s, " ");
    }
    s = concatenate(s, "else ");
    s = serialize(s, conditional.expression_else);
    return s;
}

StringBuilder serializeIs(StringBuilder s, const IsExpression& is_expression) {
    s = concatenate(s, "is ");
    s = serialize(s, is_expression.input);
    s = concatenate(s, " ");
    FOR_EACH(a, is_expression.alternative) {
        const auto alternative = storage.alternatives.data[a];
        s = serialize(s, alternative.left);
        s = concatenate(s, " then ");
        s = serialize(s, alternative.right);
        s = concatenate(s, " ");
    }
    s = concatenate(s, "else ");
    s = serialize(s, is_expression.expression_else);
    return s;
}

StringBuilder serializeDefinition(StringBuilder s, const Definition& element) {
    s = serializeName(s, element.name.global_index);
    s = concatenate(s, "=");
    s = serialize(s, element.expression);
    s = concatenate(s, " ");
    return s;
}

StringBuilder serializePutAssignment(StringBuilder s, const PutAssignment& element) {
    s = serializeName(s, element.name.global_index);
    s = concatenate(s, "+=");
    s = serialize(s, element.expression);
    s = concatenate(s, " ");
    return s;
}

StringBuilder serializePutEachAssignment(StringBuilder s, const PutEachAssignment& element) {
    s = serializeName(s, element.name.global_index);
    s = concatenate(s, "++=");
    s = serialize(s, element.expression);
    s = concatenate(s, " ");
    return s;
}

StringBuilder serializeDropAssignment(StringBuilder s, const DropAssignment& element) {
    s = serializeName(s, element.name.global_index);
    s = concatenate(s, "-- ");
    return s;
}

StringBuilder serializeWhileStatement(StringBuilder s, const WhileStatement& element) {
    s = concatenate(s, "while ");
    s = serialize(s, element.expression);
    s = concatenate(s, " ");
    return s;
}

StringBuilder serializeForStatement(StringBuilder s, const ForStatement& element) {
    s = concatenate(s, "for ");
    s = serializeName(s, element.item_name.global_index);
    s = concatenate(s, " in ");
    s = serializeName(s, element.container_name.global_index);
    s = concatenate(s, " ");
    return s;
}

StringBuilder serializeForSimpleStatement(StringBuilder s, const ForSimpleStatement& element) {
    s = concatenate(s, "for ");
    s = serializeName(s, element.container_name.global_index);
    s = concatenate(s, " ");
    return s;
}

template<typename Serializer>
StringBuilder serializeEvaluatedDictionary(StringBuilder s, Serializer serializer, const EvaluatedDictionary& dictionary) {
    if (IS_EMPTY(dictionary.definitions)) {
        s = concatenate(s, "{}");
        return s;
    }
    s = concatenate(s, "{");
    FOR_EACH(i, dictionary.definitions) {
        auto definition = storage.definitions.data[i];
        s = serializeName(s, definition.name.global_index);
        s = concatenate(s, "=");
        s = serializer(s, definition.expression);
        s = concatenate(s, " ");
    }
    LAST_ITEM(s) = '}';
    return s;
}

template<typename Serializer>
StringBuilder serializeEvaluatedTuple(StringBuilder s, Serializer serializer, Expression t) {
    const auto evaluated_tuple = storage.evaluated_tuples.data[t.index];
    if (IS_EMPTY(evaluated_tuple.indices)) {
        s = concatenate(s, "()");
        return s;
    }
    s = concatenate(s, "(");
    FOR_EACH(i, evaluated_tuple.indices) {
        const auto expression = storage.expressions.data[i];
        s = serializer(s, expression);
        s = concatenate(s, " ");
    }
    LAST_ITEM(s) = ')';
    return s;
}

StringBuilder serializeLookupChild(StringBuilder s, const LookupChild& lookup_child) {
    s = serializeName(s, lookup_child.name);
    s = concatenate(s, "@");
    s = serialize(s, lookup_child.child);
    return s;
}

StringBuilder serializeFunctionApplication(StringBuilder s, const FunctionApplication& function_application) {
    s = serializeName(s, function_application.name.global_index);
    s = concatenate(s, "!");
    s = serialize(s, function_application.child);
    return s;
}

StringBuilder serializeLookupSymbol(StringBuilder s, const LookupSymbol& lookup_symbol) {
    s = serializeName(s, lookup_symbol.name.global_index);
    return s;
}
    
StringBuilder serializeDictionary(StringBuilder s, const Dictionary& dictionary) {
    s = concatenate(s, "{");
    FOR_EACH(i, dictionary.statements) {
        const auto statement = storage.statements.data[i];
        s = serialize(s, statement);
    }
    // TODO: handle by early return.
    if (IS_EMPTY(dictionary.statements)) {
        s = concatenate(s, "}");
    }
    else {
        LAST_ITEM(s) = '}';
    }
    return s;
}

StringBuilder serializeTuple(StringBuilder s, Expression t) {
    const auto tuple_struct = storage.tuples.data[t.index];
    if (IS_EMPTY(tuple_struct.indices)) {
        s = concatenate(s, "()");
        return s;
    }
    s = concatenate(s, "(");
    FOR_EACH(i, tuple_struct.indices) {
        const auto expression = storage.expressions.data[i];
        s = serialize(s, expression);
        s = concatenate(s, " ");
    }
    LAST_ITEM(s) = ')';
    return s;
}

StringBuilder serializeStack(StringBuilder s, Expression expression) {
    s = concatenate(s, "[");
    while (expression.type != EMPTY_STACK) {
        CHECK_INTERNAL(expression.type == STACK,
            "\n\nI have found a type error.\n"
            "It happens in serializeStack.\n"
            "Instead of a stack I got a %s\n",
            getExpressionName(expression.type)
        );
        const auto stack = storage.stacks.data[expression.index];
        s = serialize(s, stack.top);
        s = concatenate(s, " ");
        expression = stack.rest;
    }
    LAST_ITEM(s) = ']';
    return s;
}

StringBuilder serializeCharacter(StringBuilder s, Character character) {
    APPEND(s, '\'');
    APPEND(s, character);
    APPEND(s, '\'');
    APPEND(s, '\0');
    DROP_BACK(s);
    // TODO: figure out why this results in segmentation faults sometimes.
    // Undefined behaviour?
    //FORMAT_STRING(s, "\'%c\'", character);
    return s;
}

StringBuilder serializeFunction(StringBuilder s, const Function& function) {
    s = concatenate(s, "in ");
    s = serializeArgument(s, storage.arguments.data[function.argument]);
    s = concatenate(s, " out ");
    s = serialize(s, function.body);
    return s;
}

StringBuilder serializeFunctionDictionary(StringBuilder s, const FunctionDictionary& function_dictionary) {
    s = concatenate(s, "in ");
    s = concatenate(s, "{");
    FOR_EACH(i, function_dictionary.arguments) {
        s = serializeArgument(s, storage.arguments.data[i]);
        s = concatenate(s, " ");
    }
    if (IS_EMPTY(function_dictionary.arguments)) {
        s = concatenate(s, "}");
    }
    else {
        LAST_ITEM(s) = '}';
    }
    s = concatenate(s, " out ");
    s = serialize(s, function_dictionary.body);
    return s;
}

StringBuilder serializeFunctionTuple(StringBuilder s, const FunctionTuple& function_stack) {
    s = concatenate(s, "in ");
    s = concatenate(s, "(");
    FOR_EACH(i, function_stack.arguments) {
        s = serializeArgument(s, storage.arguments.data[i]);
        s = concatenate(s, " ");
    }
    if (IS_EMPTY(function_stack.arguments)) {
        s = concatenate(s, ")");
    }
    else {
        LAST_ITEM(s) = ')';
    }
    s = concatenate(s, " out ");
    s = serialize(s, function_stack.body);
    return s;
}
    
StringBuilder serializeTable(StringBuilder s, Expression t) {
    auto rows = storage.tables.data[t.index].rows;
    if (IS_EMPTY(rows)) {
        return s = concatenate(s, "<>");
    }
    s = concatenate(s, "<");
    FOR_EACH(i, rows) {
        auto row = storage.rows.data[i];
        s = concatenate(s, "(");
        s = serialize(s, row.key);
        s = concatenate(s, " ");
        s = serialize(s, row.value);
        s = concatenate(s, ") ");
    }
    LAST_ITEM(s) = '>';
    return s;
}

StringBuilder serializeTypesEvaluatedTable(StringBuilder s, Expression t) {
    const auto& rows = storage.evaluated_tables.at(t.index).rows;
    if (rows.empty()) {
        s = concatenate(s, "<>");
        return s;
    }
    const auto& row = rows.begin()->second;
    s = concatenate(s, "<(");
    s = serialize_types(s, row.key);
    s = concatenate(s, " ");
    s = serialize_types(s, row.value);
    s = concatenate(s, ")>");
    return s;
}

template<typename Table>
StringBuilder serializeEvaluatedTable(StringBuilder s, const Table& table) {
    if (table.empty()) {
        s = concatenate(s, "<>");
        return s;
    }
    s = concatenate(s, "<");
    for (const auto& row : table) {
        s = concatenate(s, "(");
        s = concatenate(s, row.first.c_str());
        s = concatenate(s, " ");
        s = serialize(s, row.second.value);
        s = concatenate(s, ") ");
    }
    LAST_ITEM(s) = '>';
    return s;
}

StringBuilder serializeTypesEvaluatedStack(StringBuilder s, Expression e) {
    s = concatenate(s, "[");
    s = serialize_types(s, storage.evaluated_stacks.data[e.index].top);
    s = concatenate(s, "]");
    return s;
}

StringBuilder serializeEvaluatedStack(StringBuilder s, Expression expression) {
    s = concatenate(s, "[");
    while (expression.type != EMPTY_STACK) {
        CHECK_INTERNAL(expression.type == EVALUATED_STACK,
            "I found an internal error while serializing a stack.\n"
            "Instead of a stack I got a %s.",
            getExpressionName(expression.type)
        );
        const auto stack = storage.evaluated_stacks.data[expression.index];
        s = serialize(s, stack.top);
        s = concatenate(s, " ");
        expression = stack.rest;
    }
    LAST_ITEM(s) = ']';
    return s;
}

StringBuilder serializeNumber(StringBuilder s, Number number) {
    if (number != number) {
        s = concatenate(s, "nan");
        return s;
    }
    // TODO: figure out why this results in segmentation faults sometimes.
    // Undefined behaviour?
    //FORMAT_STRING(s, "%.*g", DBL_DIG, number);
    auto serialized_number = StringBuilder{};
    CONCAT_STRING(serialized_number, "%.*g", DBL_DIG, number);
    CONCAT_CSTRING(s, serialized_number.data);
    FREE_DARRAY(serialized_number);
    return s;
}

StringBuilder serializeString(StringBuilder s, Expression expression) {
    s = concatenate(s, "\"");
    while (expression.type != EMPTY_STRING) {
        CHECK_INTERNAL(expression.type == STRING,
            "I found an internal error while serializing a string.\n"
            "Instead of a string I got a %s",
            getExpressionName(expression.type)
        );
        const auto string = storage.strings.data[expression.index];
        const auto top = string.top;
        const auto rest = string.rest;
        CHECK_INTERNAL(top.type == CHARACTER,
            "I found an internal error while serializing a string.\n"
            "Each item in the string should be a character,\n"
            "but I found a %s",
            getExpressionName(top.type)
        );
        APPEND(s, getCharacter(top));
        expression = rest;
    }
    s = concatenate(s, "\"");
    return s;
}

StringBuilder serializeErrorMessage(StringBuilder s, const char* error_message, CodeRange range) {
    CLEAR(s);
    CONCAT_CSTRING(s, error_message);
    CONCAT_CSTRING(s, describeLocation(range));
    return s;
}

} // namespace

StringBuilder serialize_types(StringBuilder s, Expression expression) {
    switch (expression.type) {
        case PARSE_ERROR: return serializeErrorMessage(s, getParseError(expression), expression.range);
        case EVALUATE_ERROR: return serializeErrorMessage(s, getEvaluateError(expression), expression.range);

        case EVALUATED_DICTIONARY: return serializeEvaluatedDictionary(s, serialize_types, storage.evaluated_dictionaries.data[expression.index]);
        case EVALUATED_TUPLE: return serializeEvaluatedTuple(s, serialize_types, expression);
        case EVALUATED_STACK: return serializeTypesEvaluatedStack(s, expression);
        case EVALUATED_TABLE: return serializeTypesEvaluatedTable(s, expression);
        // TODO: EVALUATED_TABLE_VIEW?
        default: return concatenate(s, getExpressionName(expression.type)); return s;
    }
}

StringBuilder serialize(StringBuilder s, Expression expression) {
    switch (expression.type) {
        case PARSE_ERROR: return serializeErrorMessage(s, getParseError(expression), expression.range);
        case EVALUATE_ERROR: return serializeErrorMessage(s, getEvaluateError(expression), expression.range);

        case CHARACTER: return serializeCharacter(s, getCharacter(expression));
        case CONDITIONAL: return serializeConditional(s, storage.conditionals.data[expression.index]);
        case IS: return serializeIs(s, storage.is_expressions.data[expression.index]);
        case DICTIONARY: return serializeDictionary(s, storage.dictionaries.data[expression.index]);
        case EVALUATED_DICTIONARY: return serializeEvaluatedDictionary(s, serialize, storage.evaluated_dictionaries.data[expression.index]);
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
        case EMPTY_STACK: return concatenate(s, "[]");
        case YES: return concatenate(s, "yes");
        case NO: return concatenate(s, "no");
        case WHILE_END_STATEMENT: return concatenate(s, "end ");
        case FOR_END_STATEMENT: return concatenate(s, "end ");
        case FOR_SIMPLE_END_STATEMENT: return concatenate(s, "end ");
        case RETURN_STATEMENT: return concatenate(s, "return ");
        default: return concatenate(s, getExpressionName(expression.type));
    }
}
