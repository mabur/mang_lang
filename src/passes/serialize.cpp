#include "serialize.h"

#include <limits>
#include <sstream>

#include "../factory.h"

namespace {

SerializedString concatcstring(SerializedString base, const char* tail) {
    base.append(tail);
    return base;
}

void replaceBack(SerializedString base, char tail) {
    base.back() = tail;
}

void append(SerializedString base, char tail) {
    base += tail;
}

SerializedString serializeName(SerializedString s, size_t name) {
    concatcstring(s, storage.names.at(name).c_str());
    return s;
}

SerializedString serializeArgument(SerializedString s, Argument a) {
    if (a.type.type != ANY) {
        serialize(s, a.type);
        concatcstring(s, ":");
        serializeName(s, a.name);
        return s;
    }
    serializeName(s, a.name);
    return s;
}

SerializedString serializeDynamicExpression(SerializedString s, const DynamicExpression& dynamic_expression) {
    concatcstring(s, "dynamic ");
    serialize(s, dynamic_expression.expression);
    return s;
}

SerializedString serializeTypedExpression(SerializedString s, const TypedExpression& typed_expression) {
    serializeName(s, typed_expression.type_name.global_index);
    concatcstring(s, ":");
    serialize(s, typed_expression.value);
    return s;
}

SerializedString serializeConditional(SerializedString s, const Conditional& conditional) {
    concatcstring(s, "if ");
    for (auto a = conditional.alternative_first;
        a.index <= conditional.alternative_last.index;
        ++a.index
    ) {
        const auto alternative = storage.alternatives.data[a.index];
        serialize(s, alternative.left);
        concatcstring(s, " then ");
        serialize(s, alternative.right);
        concatcstring(s, " ");
    }
    concatcstring(s, "else ");
    serialize(s, conditional.expression_else);
    return s;
}

SerializedString serializeIs(SerializedString s, const IsExpression& is_expression) {
    concatcstring(s, "is ");
    serialize(s, is_expression.input);
    concatcstring(s, " ");
    for (auto a = is_expression.alternative_first;
        a.index <= is_expression.alternative_last.index;
        ++a.index
    ) {
        const auto alternative = storage.alternatives.data[a.index];
        serialize(s, alternative.left);
        concatcstring(s, " then ");
        serialize(s, alternative.right);
        concatcstring(s, " ");
    }
    concatcstring(s, "else ");
    serialize(s, is_expression.expression_else);
    return s;
}

SerializedString serializeDefinition(SerializedString s, const Definition& element) {
    serializeName(s, element.name.global_index);
    concatcstring(s, "=");
    serialize(s, element.expression);
    concatcstring(s, " ");
    return s;
}

SerializedString serializePutAssignment(SerializedString s, const PutAssignment& element) {
    serializeName(s, element.name.global_index);
    concatcstring(s, "+=");
    serialize(s, element.expression);
    concatcstring(s, " ");
    return s;
}

SerializedString serializePutEachAssignment(SerializedString s, const PutEachAssignment& element) {
    serializeName(s, element.name.global_index);
    concatcstring(s, "++=");
    serialize(s, element.expression);
    concatcstring(s, " ");
    return s;
}

SerializedString serializeDropAssignment(SerializedString s, const DropAssignment& element) {
    serializeName(s, element.name.global_index);
    concatcstring(s, "-- ");
    return s;
}

SerializedString serializeWhileStatement(SerializedString s, const WhileStatement& element) {
    concatcstring(s, "while ");
    serialize(s, element.expression);
    concatcstring(s, " ");
    return s;
}

SerializedString serializeForStatement(SerializedString s, const ForStatement& element) {
    concatcstring(s, "for ");
    serializeName(s, element.item_name.global_index);
    concatcstring(s, " in ");
    serializeName(s, element.container_name.global_index);
    concatcstring(s, " ");
    return s;
}

SerializedString serializeForSimpleStatement(SerializedString s, const ForSimpleStatement& element) {
    concatcstring(s, "for ");
    serializeName(s, element.container_name.global_index);
    concatcstring(s, " ");
    return s;
}

template<typename Serializer>
SerializedString serializeEvaluatedDictionary(SerializedString s, Serializer serializer, const EvaluatedDictionary& dictionary) {
    if (dictionary.definitions.empty()) {
        concatcstring(s, "{}");
        return s;
    }
    concatcstring(s, "{");
    for (const auto& pair : dictionary.definitions) {
        serializeName(s, pair.name.global_index);
        concatcstring(s, "=");
        serializer(s, pair.expression);
        concatcstring(s, " ");
    }
    replaceBack(s, '}');
    return s;
}

template<typename Serializer>
SerializedString serializeEvaluatedTuple(SerializedString s, Serializer serializer, Expression t) {
    const auto evaluated_tuple = storage.evaluated_tuples.data[t.index];
    if (evaluated_tuple.first == evaluated_tuple.last) {
        concatcstring(s, "()");
        return s;
    }
    concatcstring(s, "(");
    for (size_t i = evaluated_tuple.first; i < evaluated_tuple.last; ++i) {
        const auto expression = storage.expressions.data[i];
        serializer(s, expression);
        concatcstring(s, " ");
    }
    replaceBack(s, ')');
    return s;
}

SerializedString serializeLookupChild(SerializedString s, const LookupChild& lookup_child) {
    serializeName(s, lookup_child.name);
    concatcstring(s, "@");
    serialize(s, lookup_child.child);
    return s;
}

SerializedString serializeFunctionApplication(SerializedString s, const FunctionApplication& function_application) {
    serializeName(s, function_application.name.global_index);
    concatcstring(s, "!");
    serialize(s, function_application.child);
    return s;
}

SerializedString serializeLookupSymbol(SerializedString s, const LookupSymbol& lookup_symbol) {
    serializeName(s, lookup_symbol.name.global_index);
    return s;
}
    
SerializedString serializeDictionary(SerializedString s, const Dictionary& dictionary) {
    concatcstring(s, "{");
    for (size_t i = dictionary.statement_first; i < dictionary.statement_last; ++i) {
        const auto statement = storage.statements.data[i];
        serialize(s, statement);
    }
    // TODO: handle by early return.
    if (dictionary.statement_first == dictionary.statement_last) {
        concatcstring(s, "}");
    }
    else {
        replaceBack(s, '}');
    }
    return s;
}

SerializedString serializeTuple(SerializedString s, Expression t) {
    const auto tuple_struct = storage.tuples.data[t.index];
    if (tuple_struct.first == tuple_struct.last) {
        concatcstring(s, "()");
        return s;
    }
    concatcstring(s, "(");
    for (size_t i = tuple_struct.first; i < tuple_struct.last; ++i) {
        const auto expression = storage.expressions.data[i];
        serialize(s, expression);
        concatcstring(s, " ");
    }
    replaceBack(s, ')');
    return s;
}

SerializedString serializeStack(SerializedString s, Expression expression) {
    concatcstring(s, "[");
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
        serialize(s, stack.top);
        concatcstring(s, " ");
        expression = stack.rest;
    }
    replaceBack(s, ']');
    return s;
}

SerializedString serializeCharacter(SerializedString s, Character character) {
    append(s, '\'');
    append(s, character);
    append(s, '\'');
    return s;
}

SerializedString serializeFunction(SerializedString s, const Function& function) {
    concatcstring(s, "in ");
    serializeArgument(s, storage.arguments.data[function.argument]);
    concatcstring(s, " out ");
    serialize(s, function.body);
    return s;
}

SerializedString serializeFunctionDictionary(SerializedString s, const FunctionDictionary& function_dictionary) {
    concatcstring(s, "in ");
    concatcstring(s, "{");
    for (auto i = function_dictionary.first_argument; i < function_dictionary.last_argument; ++i) {
        serializeArgument(s, storage.arguments.data[i]);
        concatcstring(s, " ");
    }
    if (function_dictionary.first_argument == function_dictionary.last_argument) {
        concatcstring(s, "}");
    }
    else {
        replaceBack(s, '}');
    }
    concatcstring(s, " out ");
    serialize(s, function_dictionary.body);
    return s;
}

SerializedString serializeFunctionTuple(SerializedString s, const FunctionTuple& function_stack) {
    concatcstring(s, "in ");
    concatcstring(s, "(");
    for (auto i = function_stack.first_argument; i < function_stack.last_argument; ++i) {
        serializeArgument(s, storage.arguments.data[i]);
        concatcstring(s, " ");
    }
    if (function_stack.first_argument == function_stack.last_argument) {
        concatcstring(s, ")");
    }
    else {
        replaceBack(s, ')');
    }
    concatcstring(s, " out ");
    serialize(s, function_stack.body);
    return s;
}
    
SerializedString serializeTable(SerializedString s, Expression t) {
    const auto rows = storage.tables.at(t.index).rows;
    if (rows.empty()) {
        return concatcstring(s, "<>");
    }
    concatcstring(s, "<");
    for (const auto& row : rows) {
        concatcstring(s, "(");
        serialize(s, row.key);
        concatcstring(s, " ");
        serialize(s, row.value);
        concatcstring(s, ") ");
    }
    replaceBack(s, '>');
    return s;
}

SerializedString serializeTypesEvaluatedTable(SerializedString s, Expression t) {
    const auto& rows = storage.evaluated_tables.at(t.index).rows;
    if (rows.empty()) {
        concatcstring(s, "<>");
        return s;
    }
    const auto& row = rows.begin()->second;
    concatcstring(s, "<(");
    serialize_types(s, row.key);
    concatcstring(s, " ");
    serialize_types(s, row.value);
    concatcstring(s, ")>");
    return s;
}

template<typename Table>
SerializedString serializeEvaluatedTable(SerializedString s, const Table& table) {
    if (table.empty()) {
        concatcstring(s, "<>");
        return s;
    }
    concatcstring(s, "<");
    for (const auto& row : table) {
        concatcstring(s, "(");
        concatcstring(s, row.first.c_str());
        concatcstring(s, " ");
        serialize(s, row.second.value);
        concatcstring(s, ") ");
    }
    replaceBack(s, '>');
    return s;
}

SerializedString serializeTypesEvaluatedStack(SerializedString s, Expression e) {
    concatcstring(s, "[");
    serialize_types(s, storage.evaluated_stacks.data[e.index].top);
    concatcstring(s, "]");
    return s;
}

SerializedString serializeEvaluatedStack(SerializedString s, Expression expression) {
    concatcstring(s, "[");
    while (expression.type != EMPTY_STACK) {
        if (expression.type != EVALUATED_STACK) {
            throw std::runtime_error{
                "I found an error while serializing a stack. "
                "Instead of a stack I got a " + NAMES[expression.type]
            };
        }
        const auto stack = storage.evaluated_stacks.data[expression.index];
        serialize(s, stack.top);
        concatcstring(s, " ");
        expression = stack.rest;
    }
    replaceBack(s, ']');
    return s;
}

SerializedString serializeNumber(SerializedString s, Number number) {
    if (number != number) {
        concatcstring(s, "nan");
        return s;
    }
    std::stringstream stream;
    stream.precision(std::numeric_limits<double>::digits10 + 1);
    stream << number;
    concatcstring(s, stream.str().c_str());
    return s;
}

SerializedString serializeString(SerializedString s, Expression expression) {
    concatcstring(s, "\"");
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
        s.push_back(getCharacter(top));
        expression = rest;
    }
    concatcstring(s, "\"");
    return s;
}

} // namespace

SerializedString serialize_types(SerializedString s, Expression expression) {
    switch (expression.type) {
        case EVALUATED_DICTIONARY: return serializeEvaluatedDictionary(s, serialize_types, storage.evaluated_dictionaries.at(expression.index));
        case EVALUATED_TUPLE: return serializeEvaluatedTuple(s, serialize_types, expression);
        case EVALUATED_STACK: return serializeTypesEvaluatedStack(s, expression);
        case EVALUATED_TABLE: return serializeTypesEvaluatedTable(s, expression);
        // TODO: EVALUATED_TABLE_VIEW?
        default: concatcstring(s, NAMES[expression.type].c_str()); return s;
    }
}

SerializedString serialize(SerializedString s, Expression expression) {
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
        case EMPTY_STACK: return concatcstring(s, "[]");
        case YES: return concatcstring(s, "yes");
        case NO: return concatcstring(s, "no");
        case WHILE_END_STATEMENT: return concatcstring(s, "end ");
        case FOR_END_STATEMENT: return concatcstring(s, "end ");
        case FOR_SIMPLE_END_STATEMENT: return concatcstring(s, "end ");
        case RETURN_STATEMENT: return concatcstring(s, "return ");
        default: return concatcstring(s, NAMES[expression.type].c_str());
    }
}
