#include "serialize.h"

#include <carma/carma.h>

#include "../exceptions.h"
#include "../factory.h"
#include "../mang_lang_string.h"

static
StringBuilder serializeName(StringBuilder s, size_t name) {
    s = concatenate(s, storage.names.data + name);
    return s;
}

static
StringBuilder serializeArgument(StringBuilder s, Argument a) {
    if (a.type.type != ANY_VALUE) {
        s = concatenate(s, "<");
        s = serialize(s, a.type);
        s = concatenate(s, ">");
        s = serializeName(s, a.name);
        return s;
    }
    s = serializeName(s, a.name);
    return s;
}

static
StringBuilder serializeDynamicExpression(StringBuilder s, const DynamicExpression& dynamic_expression) {
    s = concatenate(s, "dynamic ");
    s = serialize(s, dynamic_expression.expression);
    return s;
}

static
StringBuilder serializeTypedExpression(StringBuilder s, const TypedExpression& typed_expression) {
    s = concatenate(s, "<");
    s = serialize(s, typed_expression.type);
    s = concatenate(s, ">");
    s = serialize(s, typed_expression.value);
    return s;
}

static
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

static
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

static
StringBuilder serializeDefinition(StringBuilder s, const Definition& element) {
    s = serializeName(s, element.name.global_index);
    s = concatenate(s, "=");
    s = serialize(s, element.expression);
    s = concatenate(s, " ");
    return s;
}

static
StringBuilder serializePutAssignment(StringBuilder s, const PutAssignment& element) {
    s = serializeName(s, element.name.global_index);
    s = concatenate(s, "+=");
    s = serialize(s, element.expression);
    s = concatenate(s, " ");
    return s;
}

static
StringBuilder serializePutEachAssignment(StringBuilder s, const PutEachAssignment& element) {
    s = serializeName(s, element.name.global_index);
    s = concatenate(s, "++=");
    s = serialize(s, element.expression);
    s = concatenate(s, " ");
    return s;
}

static
StringBuilder serializeDropAssignment(StringBuilder s, const DropAssignment& element) {
    s = serializeName(s, element.name.global_index);
    s = concatenate(s, "-- ");
    return s;
}

static
StringBuilder serializeWhileStatement(StringBuilder s, const WhileStatement& element) {
    s = concatenate(s, "while ");
    s = serialize(s, element.expression);
    s = concatenate(s, " ");
    return s;
}

static
StringBuilder serializeForStatement(StringBuilder s, const ForStatement& element) {
    s = concatenate(s, "for ");
    s = serializeName(s, element.name.global_index);
    s = concatenate(s, " in ");
    s = serialize(s, element.container_expression);
    s = concatenate(s, " ");
    return s;
}

static
StringBuilder serializeIfStatement(StringBuilder s, const IfStatement& element) {
    s = concatenate(s, "if ");
    s = serialize(s, element.expression);
    s = concatenate(s, " ");
    return s;
}

template<typename Serializer>
static
StringBuilder serializeDictionaryValue(StringBuilder s, Serializer serializer, const DictionaryValue& dictionary) {
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
static
StringBuilder serializeTupleValue(StringBuilder s, Serializer serializer, Expression t) {
    const auto evaluated_tuple = storage.tuple_values.data[t.index];
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

static
StringBuilder serializeLookupChild(StringBuilder s, const LookupChild& lookup_child) {
    s = serializeName(s, lookup_child.name);
    s = concatenate(s, "@");
    s = serialize(s, lookup_child.child);
    return s;
}

static
StringBuilder serializeFunctionApplication(StringBuilder s, const FunctionApplication& function_application) {
    s = serializeName(s, function_application.name.global_index);
    s = concatenate(s, "!");
    s = serialize(s, function_application.child);
    return s;
}

static
StringBuilder serializeFunctionApplicationBuiltIn(StringBuilder s, const FunctionApplicationBuiltIn& built_in_application) {
    s = serializeName(s, built_in_application.name);
    s = concatenate(s, "!");
    s = serialize(s, built_in_application.child);
    return s;
}

static
StringBuilder serializeLookupSymbol(StringBuilder s, const LookupSymbol& lookup_symbol) {
    s = serializeName(s, lookup_symbol.name.global_index);
    return s;
}
    
static
StringBuilder serializeDictionaryExpression(StringBuilder s, const DictionaryExpression& dictionary) {
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

static
StringBuilder serializeTupleExpression(StringBuilder s, Expression t) {
    const auto tuple_struct = storage.tuple_expressions.data[t.index];
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

static
StringBuilder serializeStackExpression(StringBuilder s, Expression expression) {
    s = concatenate(s, "[");
    while (expression.type != EMPTY_STACK) {
        CHECK_INTERNAL(expression.type == STACK_EXPRESSION,
            "\n\nI have found a type error.\n"
            "It happens in serializeStackExpression.\n"
            "Instead of a stack I got a %s\n",
            getExpressionName(expression.type)
        );
        const auto stack = storage.stack_expressions.data[expression.index];
        s = serialize(s, stack.top);
        s = concatenate(s, " ");
        expression = stack.rest;
    }
    LAST_ITEM(s) = ']';
    return s;
}

static
StringBuilder serializeCharacter(StringBuilder s, Character character) {
    APPEND(s, '\'');
    APPEND(s, character);
    APPEND(s, '\'');
    APPEND(s, '\0');
    DROP_BACK(s);
    return s;
}

static
StringBuilder serializeFunctionExpression(StringBuilder s, const FunctionExpression& function) {
    s = concatenate(s, "in ");
    s = serializeArgument(s, storage.arguments.data[function.argument]);
    s = concatenate(s, " out ");
    s = serialize(s, function.body);
    return s;
}

static
StringBuilder serializeFunctionDictionaryExpression(StringBuilder s, const FunctionDictionaryExpression& function_dictionary) {
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

static
StringBuilder serializeFunctionTupleExpression(StringBuilder s, const FunctionTupleExpression& function_stack) {
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
    
static
StringBuilder serializeTableExpression(StringBuilder s, Expression t) {
    auto rows = storage.table_expressions.data[t.index].rows;
    if (IS_EMPTY(rows)) {
        return s = concatenate(s, "table[]");
    }
    s = concatenate(s, "table[");
    FOR_EACH(i, rows) {
        auto row = storage.rows.data[i];
        s = concatenate(s, "(");
        s = serialize(s, row.key);
        s = concatenate(s, " ");
        s = serialize(s, row.value);
        s = concatenate(s, ") ");
    }
    LAST_ITEM(s) = ']';
    return s;
}

static
StringBuilder serializeTypesTableValue(StringBuilder s, Expression t) {
    const auto& rows = storage.table_values.at(t.index).rows;
    if (rows.empty()) {
        s = concatenate(s, "table[]");
        return s;
    }
    const auto& row = rows.begin()->second;
    s = concatenate(s, "table[(");
    s = serialize_types(s, row.key);
    s = concatenate(s, " ");
    s = serialize_types(s, row.value);
    s = concatenate(s, ")]");
    return s;
}

template<typename TableLike>
static
StringBuilder serializeTableValue(StringBuilder s, const TableLike& table) {
    if (table.empty()) {
        s = concatenate(s, "table[]");
        return s;
    }
    s = concatenate(s, "table[");
    for (const auto& row : table) {
        s = concatenate(s, "(");
        s = concatenate(s, row.first.c_str());
        s = concatenate(s, " ");
        s = serialize(s, row.second.value);
        s = concatenate(s, ") ");
    }
    LAST_ITEM(s) = ']';
    return s;
}

static
StringBuilder serializeTypesStackValue(StringBuilder s, Expression e) {
    s = concatenate(s, "[");
    s = serialize_types(s, storage.stack_values.data[e.index].top);
    s = concatenate(s, "]");
    return s;
}

static
StringBuilder serializeStackValue(StringBuilder s, Expression expression) {
    s = concatenate(s, "[");
    while (expression.type != EMPTY_STACK) {
        CHECK_INTERNAL(expression.type == STACK_VALUE,
            "I found an internal error while serializing a stack.\n"
            "Instead of a stack I got a %s.",
            getExpressionName(expression.type)
        );
        const auto stack = storage.stack_values.data[expression.index];
        s = serialize(s, stack.top);
        s = concatenate(s, " ");
        expression = stack.rest;
    }
    LAST_ITEM(s) = ']';
    return s;
}

static
StringBuilder serializeNumber(StringBuilder s, Number number) {
    if (number != number) {
        s = concatenate(s, "nan");
        return s;
    }
    SERIALIZE_DOUBLE(s, number);
    return s;
}

static
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

static
StringBuilder serializeErrorMessage(StringBuilder s, const char* error_message, CodeRange range) {
    (void)range;
    CLEAR(s);
    SERIALIZE_CSTRING(s, error_message);
    return s;
}

StringBuilder serialize_types(StringBuilder s, Expression expression) {
    switch (expression.type) {
        case ERROR_VALUE: return serializeErrorMessage(s, getErrorValue(expression), expression.range);

        case DICTIONARY_VALUE: return serializeDictionaryValue(s, serialize_types, storage.dictionary_values.data[expression.index]);
        case TUPLE_VALUE: return serializeTupleValue(s, serialize_types, expression);
        case STACK_VALUE: return serializeTypesStackValue(s, expression);
        case TABLE_VALUE: return serializeTypesTableValue(s, expression);
        case FUNCTION_VALUE: return concatenate(s, getFunctionTypeName(storage.function_values.data[expression.index].function.type));
        case ANY_VALUE: return concatenate(s, "ANY");
        // TODO: TABLE_VIEW_VALUE?
        default: return concatenate(s, getExpressionName(expression.type)); return s;
    }
}

StringBuilder serialize(StringBuilder s, Expression expression) {
    switch (expression.type) {
        case ERROR_VALUE: return serializeErrorMessage(s, getErrorValue(expression), expression.range);

        case CHARACTER: return serializeCharacter(s, getCharacter(expression));
        case CONDITIONAL: return serializeConditional(s, storage.conditionals.data[expression.index]);
        case IS: return serializeIs(s, storage.is_expressions.data[expression.index]);
        case DICTIONARY_EXPRESSION: return serializeDictionaryExpression(s, storage.dictionary_expressions.data[expression.index]);
        case DICTIONARY_VALUE: return serializeDictionaryValue(s, serialize, storage.dictionary_values.data[expression.index]);
        case DEFINITION: return serializeDefinition(s, storage.definitions.data[expression.index]);
        case PUT_ASSIGNMENT: return serializePutAssignment(s, storage.put_assignments.data[expression.index]);
        case PUT_EACH_ASSIGNMENT: return serializePutEachAssignment(s, storage.put_each_assignments.data[expression.index]);
        case DROP_ASSIGNMENT: return serializeDropAssignment(s, storage.drop_assignments.data[expression.index]);
        case WHILE_STATEMENT: return serializeWhileStatement(s, storage.while_statements.data[expression.index]);
        case FOR_INIT_STATEMENT: return s; // Handled by the FOR_STATEMENT that comes right after
        case FOR_STATEMENT: return serializeForStatement(s, storage.for_statements.data[expression.index]);
        case IF_STATEMENT: return serializeIfStatement(s, storage.if_statements.data[expression.index]);
        case FUNCTION_EXPRESSION: return serializeFunctionExpression(s, storage.function_expressions.data[expression.index]);
        case FUNCTION_DICTIONARY_EXPRESSION: return serializeFunctionDictionaryExpression(s, storage.function_dictionary_expressions.data[expression.index]);
        case FUNCTION_TUPLE_EXPRESSION: return serializeFunctionTupleExpression(s, storage.function_tuple_expressions.data[expression.index]);
        case FUNCTION_VALUE: return serialize(s, storage.function_values.data[expression.index].function);
        case TABLE_EXPRESSION: return serializeTableExpression(s, expression);
        case TABLE_VALUE: return serializeTableValue(s, storage.table_values.at(expression.index).rows);
        case TABLE_VIEW_VALUE: return serializeTableValue(s, storage.table_view_values.data[expression.index]);
        case TUPLE_EXPRESSION: return serializeTupleExpression(s, expression);
        case TUPLE_VALUE: return serializeTupleValue(s, serialize, expression);
        case STACK_EXPRESSION: return serializeStackExpression(s, expression);
        case STACK_VALUE: return serializeStackValue(s, expression);
        case LOOKUP_CHILD: return serializeLookupChild(s, storage.child_lookups.data[expression.index]);
        case FUNCTION_APPLICATION: return serializeFunctionApplication(s, storage.function_applications.data[expression.index]);
        case FUNCTION_APPLICATION_BUILT_IN: return serializeFunctionApplicationBuiltIn(s, storage.function_applications_built_in.data[expression.index]);
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
        case IF_END_STATEMENT: return concatenate(s, "end ");
        case END_STATEMENT: return concatenate(s, "end ");
        case RETURN_STATEMENT: return concatenate(s, "return ");
        case ANY_VALUE: return concatenate(s, "ANY");
        default: return concatenate(s, getExpressionName(expression.type));
    }
}
