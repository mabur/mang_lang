#include "serialize.h"

#include <limits>
#include <sstream>

#include "../factory.h"

namespace {

void concatcstring(SerializedString base, const char* tail) {
    base.append(tail);
}

void serializeName(SerializedString s, size_t name) {
    concatcstring(s, storage.names.at(name).c_str());
}

void serializeArgument(SerializedString s, Argument a) {
    if (a.type.type != ANY) {
        serialize(s, a.type);
        concatcstring(s, ":");
        serializeName(s, a.name);
        return;
    }
    serializeName(s, a.name);
}

void serializeDynamicExpression(SerializedString s, const DynamicExpression& dynamic_expression) {
    concatcstring(s, "dynamic ");
    serialize(s, dynamic_expression.expression);
}

void serializeTypedExpression(SerializedString s, const TypedExpression& typed_expression) {
    serializeName(s, typed_expression.type_name.global_index);
    concatcstring(s, ":");
    serialize(s, typed_expression.value);
}

void serializeConditional(SerializedString s, const Conditional& conditional) {
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
}

void serializeIs(SerializedString s, const IsExpression& is_expression) {
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
}

void serializeDefinition(SerializedString s, const Definition& element) {
    serializeName(s, element.name.global_index);
    concatcstring(s, "=");
    serialize(s, element.expression);
    concatcstring(s, " ");
}

void serializePutAssignment(SerializedString s, const PutAssignment& element) {
    serializeName(s, element.name.global_index);
    concatcstring(s, "+=");
    serialize(s, element.expression);
    concatcstring(s, " ");
}

void serializePutEachAssignment(SerializedString s, const PutEachAssignment& element) {
    serializeName(s, element.name.global_index);
    concatcstring(s, "++=");
    serialize(s, element.expression);
    concatcstring(s, " ");
}

void serializeDropAssignment(SerializedString s, const DropAssignment& element) {
    serializeName(s, element.name.global_index);
    concatcstring(s, "-- ");
}

void serializeWhileStatement(SerializedString s, const WhileStatement& element) {
    concatcstring(s, "while ");
    serialize(s, element.expression);
    concatcstring(s, " ");
}

void serializeForStatement(SerializedString s, const ForStatement& element) {
    concatcstring(s, "for ");
    serializeName(s, element.item_name.global_index);
    concatcstring(s, " in ");
    serializeName(s, element.container_name.global_index);
    concatcstring(s, " ");
}

void serializeForSimpleStatement(SerializedString s, const ForSimpleStatement& element) {
    concatcstring(s, "for ");
    serializeName(s, element.container_name.global_index);
    concatcstring(s, " ");
}

template<typename Serializer>
void serializeEvaluatedDictionary(SerializedString s, Serializer serializer, const EvaluatedDictionary& dictionary) {
    if (dictionary.definitions.empty()) {
        concatcstring(s, "{}");
        return;
    }
    concatcstring(s, "{");
    for (const auto& pair : dictionary.definitions) {
        serializeName(s, pair.name.global_index);
        concatcstring(s, "=");
        serializer(s, pair.expression);
        concatcstring(s, " ");
    }
    s.back() = '}';
}

template<typename Serializer>
void serializeEvaluatedTuple(SerializedString s, Serializer serializer, Expression t) {
    const auto evaluated_tuple = storage.evaluated_tuples.data[t.index];
    if (evaluated_tuple.first == evaluated_tuple.last) {
        concatcstring(s, "()");
        return;
    }
    concatcstring(s, "(");
    for (size_t i = evaluated_tuple.first; i < evaluated_tuple.last; ++i) {
        const auto expression = storage.expressions.data[i];
        serializer(s, expression);
        concatcstring(s, " ");
    }
    s.back() = ')';
}

void serializeLookupChild(SerializedString s, const LookupChild& lookup_child) {
    serializeName(s, lookup_child.name);
    concatcstring(s, "@");
    serialize(s, lookup_child.child);
}

void serializeFunctionApplication(SerializedString s, const FunctionApplication& function_application) {
    serializeName(s, function_application.name.global_index);
    concatcstring(s, "!");
    serialize(s, function_application.child);
}

void serializeLookupSymbol(SerializedString s, const LookupSymbol& lookup_symbol) {
    serializeName(s, lookup_symbol.name.global_index);
}
    
void serializeDictionary(SerializedString s, const Dictionary& dictionary) {
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
        s.back() = '}';
    }
}

void serializeTuple(SerializedString s, Expression t) {
    const auto tuple_struct = storage.tuples.data[t.index];
    if (tuple_struct.first == tuple_struct.last) {
        concatcstring(s, "()");
        return;
    }
    concatcstring(s, "(");
    for (size_t i = tuple_struct.first; i < tuple_struct.last; ++i) {
        const auto expression = storage.expressions.data[i];
        serialize(s, expression);
        concatcstring(s, " ");
    }
    s.back() = ')';
}

void serializeStack(SerializedString s, Expression expression) {
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
    s.back() = ']';
}

void serializeCharacter(SerializedString s, Character character) {
    s += '\'';
    s += character;
    s += '\'';
}

void serializeFunction(SerializedString s, const Function& function) {
    concatcstring(s, "in ");
    serializeArgument(s, storage.arguments.data[function.argument]);
    concatcstring(s, " out ");
    serialize(s, function.body);
}

void serializeFunctionDictionary(SerializedString s, const FunctionDictionary& function_dictionary) {
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
        s.back() = '}';
    }
    concatcstring(s, " out ");
    serialize(s, function_dictionary.body);
}

void serializeFunctionTuple(SerializedString s, const FunctionTuple& function_stack) {
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
        s.back() = ')';
    }
    concatcstring(s, " out ");
    serialize(s, function_stack.body);
}
    
void serializeTable(SerializedString s, Expression t) {
    const auto rows = storage.tables.at(t.index).rows;
    if (rows.empty()) {
        concatcstring(s, "<>");
        return;
    }
    concatcstring(s, "<");
    for (const auto& row : rows) {
        concatcstring(s, "(");
        serialize(s, row.key);
        concatcstring(s, " ");
        serialize(s, row.value);
        concatcstring(s, ") ");
    }
    s.back() = '>';
}

void serializeTypesEvaluatedTable(SerializedString s, Expression t) {
    const auto& rows = storage.evaluated_tables.at(t.index).rows;
    if (rows.empty()) {
        concatcstring(s, "<>");
        return;
    }
    const auto& row = rows.begin()->second;
    concatcstring(s, "<(");
    serialize_types(s, row.key);
    concatcstring(s, " ");
    serialize_types(s, row.value);
    concatcstring(s, ")>");
}

template<typename Table>
void serializeEvaluatedTable(SerializedString s, const Table& table) {
    if (table.empty()) {
        concatcstring(s, "<>");
        return;
    }
    concatcstring(s, "<");
    for (const auto& row : table) {
        concatcstring(s, "(");
        s.append(row.first);
        concatcstring(s, " ");
        serialize(s, row.second.value);
        concatcstring(s, ") ");
    }
    s.back() = '>';
}

void serializeTypesEvaluatedStack(SerializedString s, Expression e) {
    concatcstring(s, "[");
    serialize_types(s, storage.evaluated_stacks.data[e.index].top);
    concatcstring(s, "]");
}

void serializeEvaluatedStack(SerializedString s, Expression expression) {
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
    s.back() = ']';
}

void serializeNumber(SerializedString s, Number number) {
    if (number != number) {
        concatcstring(s, "nan");
        return;
    }
    std::stringstream stream;
    stream.precision(std::numeric_limits<double>::digits10 + 1);
    stream << number;
    s.append(stream.str());
}

void serializeString(SerializedString s, Expression expression) {
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
}

} // namespace

void serialize_types(SerializedString s, Expression expression) {
    switch (expression.type) {
        case EVALUATED_DICTIONARY: serializeEvaluatedDictionary(s, serialize_types, storage.evaluated_dictionaries.at(expression.index)); return;
        case EVALUATED_TUPLE: serializeEvaluatedTuple(s, serialize_types, expression); return;
        case EVALUATED_STACK: serializeTypesEvaluatedStack(s, expression); return;
        case EVALUATED_TABLE: serializeTypesEvaluatedTable(s, expression); return;
        // TODO: EVALUATED_TABLE_VIEW?
        default: s.append(NAMES[expression.type]); return;
    }
}

void serialize(SerializedString s, Expression expression) {
    switch (expression.type) {
        case CHARACTER: serializeCharacter(s, getCharacter(expression)); return;
        case CONDITIONAL: serializeConditional(s, storage.conditionals.data[expression.index]); return;
        case IS: serializeIs(s, storage.is_expressions.data[expression.index]); return;
        case DICTIONARY: serializeDictionary(s, storage.dictionaries.data[expression.index]); return;
        case EVALUATED_DICTIONARY: serializeEvaluatedDictionary(s, serialize, storage.evaluated_dictionaries.at(expression.index)); return;
        case DEFINITION: serializeDefinition(s, storage.definitions.data[expression.index]); return;
        case PUT_ASSIGNMENT: serializePutAssignment(s, storage.put_assignments.data[expression.index]); return;
        case PUT_EACH_ASSIGNMENT: serializePutEachAssignment(s, storage.put_each_assignments.data[expression.index]); return;
        case DROP_ASSIGNMENT: serializeDropAssignment(s, storage.drop_assignments.data[expression.index]); return;
        case WHILE_STATEMENT: serializeWhileStatement(s, storage.while_statements.data[expression.index]); return;
        case FOR_STATEMENT: serializeForStatement(s, storage.for_statements.data[expression.index]); return;
        case FOR_SIMPLE_STATEMENT: serializeForSimpleStatement(s, storage.for_simple_statements.data[expression.index]); return;
        case FUNCTION: serializeFunction(s, storage.functions.data[expression.index]); return;
        case FUNCTION_DICTIONARY: serializeFunctionDictionary(s, storage.dictionary_functions.data[expression.index]); return;
        case FUNCTION_TUPLE: serializeFunctionTuple(s, storage.tuple_functions.data[expression.index]); return;
        case TABLE: serializeTable(s, expression); return;
        case EVALUATED_TABLE: serializeEvaluatedTable(s, storage.evaluated_tables.at(expression.index).rows); return;
        case EVALUATED_TABLE_VIEW: serializeEvaluatedTable(s, storage.evaluated_table_views.data[expression.index]); return;
        case TUPLE: serializeTuple(s, expression); return;
        case EVALUATED_TUPLE: serializeEvaluatedTuple(s, serialize, expression); return;
        case STACK: serializeStack(s, expression); return;
        case EVALUATED_STACK: serializeEvaluatedStack(s, expression); return;
        case LOOKUP_CHILD: serializeLookupChild(s, storage.child_lookups.data[expression.index]); return;
        case FUNCTION_APPLICATION: serializeFunctionApplication(s, storage.function_applications.data[expression.index]); return;
        case LOOKUP_SYMBOL: serializeLookupSymbol(s, storage.symbol_lookups.data[expression.index]); return;
        case NUMBER: serializeNumber(s, getNumber(expression)); return;
        case EMPTY_STRING: serializeString(s, expression); return;
        case STRING: serializeString(s, expression); return;
        case DYNAMIC_EXPRESSION: serializeDynamicExpression(s, storage.dynamic_expressions.data[expression.index]); return;
        case TYPED_EXPRESSION: serializeTypedExpression(s, storage.typed_expressions.data[expression.index]); return;
        case EMPTY_STACK: concatcstring(s, "[]"); return;
        case YES: concatcstring(s, "yes"); return;
        case NO: concatcstring(s, "no"); return;
        case WHILE_END_STATEMENT: concatcstring(s, "end "); return;
        case FOR_END_STATEMENT: concatcstring(s, "end "); return;
        case FOR_SIMPLE_END_STATEMENT: concatcstring(s, "end "); return;
        case RETURN_STATEMENT: concatcstring(s, "return "); return;
        default: s.append(NAMES[expression.type]); return;
    }
}
