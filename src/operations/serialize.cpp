#include "serialize.h"

#include <limits>
#include <sstream>

#include "../factory.h"

namespace {

void serializeName(std::string& s, size_t name) {
    s.append(storage.names.at(name));
}

void serializeArgument(std::string& s, Argument a) {
    if (a.type.type != ANY) {
        serialize(s, a.type);
        s.append(":");
        serializeName(s, a.name.index);
        return;
    }
    serializeName(s, a.name.index);
}

void serializeDynamicExpression(std::string& s, const DynamicExpression& dynamic_expression) {
    s.append("dynamic ");
    serialize(s, dynamic_expression.expression);
}

void serializeTypedExpression(std::string& s, const TypedExpression& typed_expression) {
    serialize(s, typed_expression.type);
    s.append(":");
    serialize(s, typed_expression.value);
}

void serializeConditional(std::string& s, const Conditional& conditional) {
    s.append("if ");
    for (auto a = conditional.alternative_first;
        a.index <= conditional.alternative_last.index;
        ++a.index
    ) {
        const auto alternative = storage.alternatives.at(a.index);
        serialize(s, alternative.left);
        s.append(" then ");
        serialize(s, alternative.right);
        s.append(" ");
    }
    s.append("else ");
    serialize(s, conditional.expression_else);
}

void serializeIs(std::string& s, const IsExpression& is_expression) {
    s.append("is ");
    serialize(s, is_expression.input);
    s.append(" ");
    for (auto a = is_expression.alternative_first;
        a.index <= is_expression.alternative_last.index;
        ++a.index
    ) {
        const auto alternative = storage.alternatives.at(a.index);
        serialize(s, alternative.left);
        s.append(" then ");
        serialize(s, alternative.right);
        s.append(" ");
    }
    s.append("else ");
    serialize(s, is_expression.expression_else);
}

void serializeDefinition(std::string& s, const Definition& element) {
    serializeName(s, element.name.index);
    s.append("=");
    serialize(s, element.expression);
    s.append(" ");
}

void serializePutAssignment(std::string& s, const PutAssignment& element) {
    serializeName(s, element.name.index);
    s.append("+=");
    serialize(s, element.expression);
    s.append(" ");
}

void serializePutEachAssignment(std::string& s, const PutEachAssignment& element) {
    serializeName(s, element.name.index);
    s.append("++=");
    serialize(s, element.expression);
    s.append(" ");
}

void serializeDropAssignment(std::string& s, const DropAssignment& element) {
    serializeName(s, element.name.index);
    s.append("-- ");
}

void serializeWhileStatement(std::string& s, const WhileStatement& element) {
    s.append("while ");
    serialize(s, element.expression);
    s.append(" ");
}

void serializeForStatement(std::string& s, const ForStatement& element) {
    s.append("for ");
    serializeName(s, element.name_item.index);
    s.append(" in ");
    serializeName(s, element.name_container.index);
    s.append(" ");
}

void serializeForSimpleStatement(std::string& s, const ForSimpleStatement& element) {
    s.append("for ");
    serializeName(s, element.name_container.index);
    s.append(" ");
}

template<typename Serializer>
void serializeEvaluatedDictionary(std::string& s, Serializer serializer, const EvaluatedDictionary& dictionary) {
    if (dictionary.definitions.empty()) {
        s.append("{}");
        return;
    }
    s.append("{");
    for (const auto& pair : dictionary.definitions) {
        serializeName(s, pair.name.index);
        s.append("=");
        serializer(s, pair.expression);
        s.append(" ");
    }
    s.back() = '}';
}

template<typename Serializer>
void serializeEvaluatedTuple(std::string& s, Serializer serializer, Expression t) {
    const auto& expressions = storage.evaluated_tuples.at(t.index).expressions;
    if (expressions.empty()) {
        s.append("()");
        return;
    }
    s.append("(");
    for (const auto& expression : expressions) {
        serializer(s, expression);
        s.append(" ");
    }
    s.back() = ')';
}

void serializeLookupChild(std::string& s, const LookupChild& lookup_child) {
    serializeName(s, lookup_child.name);
    s.append("@");
    serialize(s, lookup_child.child);
}

void serializeFunctionApplication(std::string& s, const FunctionApplication& function_application) {
    serializeName(s, function_application.name);
    s.append("!");
    serialize(s, function_application.child);
}

void serializeLookupSymbol(std::string& s, const LookupSymbol& lookup_symbol) {
    serializeName(s, lookup_symbol.name.index);
}
    
void serializeDictionary(std::string& s, const Dictionary& dictionary) {
    s.append("{");
    for (const auto& statement : dictionary.statements) {
        serialize(s, statement);
    }
    if (dictionary.statements.empty()) {
        s.append("}");
    }
    else {
        s.back() = '}';
    }
}

void serializeTuple(std::string& s, Expression t) {
    const auto expressions = storage.tuples.at(t.index).expressions;
    if (expressions.empty()) {
        s.append("()");
        return;
    }
    s.append("(");
    for (const auto& expression : expressions) {
        serialize(s, expression);
        s.append(" ");
    }
    s.back() = ')';
}

void serializeStack(std::string& s, Expression expression) {
    s.append("[");
    while (expression.type != EMPTY_STACK) {
        if (expression.type != STACK) {
            throw std::runtime_error(
                std::string{"\n\nI have found a type error."} +
                    "\nIt happens in serializeStack. " +
                    "\nInstead of a stack I got a " + NAMES[expression.type] +
                    ".\n"
            );
        }
        const auto stack = storage.stacks.at(expression.index);
        serialize(s, stack.top);
        s.append(" ");
        expression = stack.rest;
    }
    s.back() = ']';
}

void serializeCharacter(std::string& s, Character character) {
    s += '\'';
    s += character;
    s += '\'';
}

void serializeFunction(std::string& s, const Function& function) {
    s.append("in ");
    serializeArgument(s, storage.arguments.at(function.argument.index));
    s.append(" out ");
    serialize(s, function.body);
}

void serializeFunctionDictionary(std::string& s, const FunctionDictionary& function_dictionary) {
    s.append("in ");
    s.append("{");
    for (auto i = function_dictionary.first_argument.index; i < function_dictionary.last_argument.index; ++i) {
        serializeArgument(s, storage.arguments.at(i));
        s.append(" ");
    }
    if (function_dictionary.first_argument.index == function_dictionary.last_argument.index) {
        s.append("}");
    }
    else {
        s.back() = '}';
    }
    s.append(" out ");
    serialize(s, function_dictionary.body);
}

void serializeFunctionTuple(std::string& s, const FunctionTuple& function_stack) {
    s.append("in ");
    s.append("(");
    for (auto i = function_stack.first_argument.index; i < function_stack.last_argument.index; ++i) {
        serializeArgument(s, storage.arguments.at(i));
        s.append(" ");
    }
    if (function_stack.first_argument.index == function_stack.last_argument.index) {
        s.append(")");
    }
    else {
        s.back() = ')';
    }
    s.append(" out ");
    serialize(s, function_stack.body);
}
    
void serializeTable(std::string& s, Expression t) {
    const auto rows = storage.tables.at(t.index).rows;
    if (rows.empty()) {
        s.append("<>");
        return;
    }
    s.append("<");
    for (const auto& row : rows) {
        s.append("(");
        serialize(s, row.key);
        s.append(" ");
        serialize(s, row.value);
        s.append(") ");
    }
    s.back() = '>';
}

void serializeTypesEvaluatedTable(std::string& s, Expression t) {
    const auto& rows = storage.evaluated_tables.at(t.index).rows;
    if (rows.empty()) {
        s.append("<>");
        return;
    }
    const auto& row = rows.begin()->second;
    s.append("<(");
    serialize_types(s, row.key);
    s.append(" ");
    serialize_types(s, row.value);
    s.append(")>");
}

template<typename Table>
void serializeEvaluatedTable(std::string& s, const Table& table) {
    if (table.empty()) {
        s.append("<>");
        return;
    }
    s.append("<");
    for (const auto& row : table) {
        s.append("(");
        s.append(row.first);
        s.append(" ");
        serialize(s, row.second.value);
        s.append(") ");
    }
    s.back() = '>';
}

void serializeTypesEvaluatedStack(std::string& s, Expression e) {
    s.append("[");
    serialize_types(s, storage.evaluated_stacks.at(e.index).top);
    s.append("]");
}

void serializeEvaluatedStack(std::string& s, Expression expression) {
    s.append("[");
    while (expression.type != EMPTY_STACK) {
        if (expression.type != EVALUATED_STACK) {
            throw std::runtime_error{
                "I found an error while serializing a stack. "
                "Instead of a stack I got a " + NAMES[expression.type]
            };
        }
        const auto stack = storage.evaluated_stacks.at(expression.index);
        serialize(s, stack.top);
        s.append(" ");
        expression = stack.rest;
    }
    s.back() = ']';
}

void serializeNumber(std::string& s, Number number) {
    if (number != number) {
        s.append("nan");
        return;
    }
    std::stringstream stream;
    stream.precision(std::numeric_limits<double>::digits10 + 1);
    stream << number;
    s.append(stream.str());
}

void serializeString(std::string& s, Expression expression) {
    s.append("\"");
    while (expression.type != EMPTY_STRING) {
        if (expression.type != STRING) {
            throw std::runtime_error{
                "I found an error while serializing a string. "
                "Instead of a string I got a " + NAMES[expression.type]
            };
        }
        const auto string = storage.strings.at(expression.index);
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
    s.append("\"");
}

} // namespace

void serialize_types(std::string& s, Expression expression) {
    switch (expression.type) {
        case EVALUATED_DICTIONARY: serializeEvaluatedDictionary(s, serialize_types, storage.evaluated_dictionaries.at(expression.index)); return;
        case EVALUATED_TUPLE: serializeEvaluatedTuple(s, serialize_types, expression); return;
        case EVALUATED_STACK: serializeTypesEvaluatedStack(s, expression); return;
        case EVALUATED_TABLE: serializeTypesEvaluatedTable(s, expression); return;
        // TODO: EVALUATED_TABLE_VIEW?
        default: s.append(NAMES[expression.type]); return;
    }
}

void serialize(std::string& s, Expression expression) {
    switch (expression.type) {
        case CHARACTER: serializeCharacter(s, getCharacter(expression)); return;
        case CONDITIONAL: serializeConditional(s, storage.conditionals.at(expression.index)); return;
        case IS: serializeIs(s, storage.is_expressions.at(expression.index)); return;
        case DICTIONARY: serializeDictionary(s, storage.dictionaries.at(expression.index)); return;
        case EVALUATED_DICTIONARY: serializeEvaluatedDictionary(s, serialize, storage.evaluated_dictionaries.at(expression.index)); return;
        case DEFINITION: serializeDefinition(s, storage.definitions.at(expression.index)); return;
        case PUT_ASSIGNMENT: serializePutAssignment(s, storage.put_assignments.at(expression.index)); return;
        case PUT_EACH_ASSIGNMENT: serializePutEachAssignment(s, storage.put_each_assignments.at(expression.index)); return;
        case DROP_ASSIGNMENT: serializeDropAssignment(s, storage.drop_assignments.at(expression.index)); return;
        case WHILE_STATEMENT: serializeWhileStatement(s, storage.while_statements.at(expression.index)); return;
        case FOR_STATEMENT: serializeForStatement(s, storage.for_statements.at(expression.index)); return;
        case FOR_SIMPLE_STATEMENT: serializeForSimpleStatement(s, storage.for_simple_statements.at(expression.index)); return;
        case FUNCTION: serializeFunction(s, storage.functions.at(expression.index)); return;
        case FUNCTION_DICTIONARY: serializeFunctionDictionary(s, storage.dictionary_functions.at(expression.index)); return;
        case FUNCTION_TUPLE: serializeFunctionTuple(s, storage.tuple_functions.at(expression.index)); return;
        case TABLE: serializeTable(s, expression); return;
        case EVALUATED_TABLE: serializeEvaluatedTable(s, storage.evaluated_tables.at(expression.index).rows); return;
        case EVALUATED_TABLE_VIEW: serializeEvaluatedTable(s, storage.evaluated_table_views.at(expression.index)); return;
        case TUPLE: serializeTuple(s, expression); return;
        case EVALUATED_TUPLE: serializeEvaluatedTuple(s, serialize, expression); return;
        case STACK: serializeStack(s, expression); return;
        case EVALUATED_STACK: serializeEvaluatedStack(s, expression); return;
        case LOOKUP_CHILD: serializeLookupChild(s, storage.child_lookups.at(expression.index)); return;
        case FUNCTION_APPLICATION: serializeFunctionApplication(s, storage.function_applications.at(expression.index)); return;
        case LOOKUP_SYMBOL: serializeLookupSymbol(s, storage.symbol_lookups.at(expression.index)); return;
        case NUMBER: serializeNumber(s, storage.numbers.at(expression.index)); return;
        case EMPTY_STRING: serializeString(s, expression); return;
        case STRING: serializeString(s, expression); return;
        case DYNAMIC_EXPRESSION: serializeDynamicExpression(s, storage.dynamic_expressions.at(expression.index)); return;
        case TYPED_EXPRESSION: serializeTypedExpression(s, storage.typed_expressions.at(expression.index)); return;
        case EMPTY_STACK: s.append("[]"); return;
        case YES: s.append("yes"); return;
        case NO: s.append("no"); return;
        case WHILE_END_STATEMENT: s.append("end "); return;
        case FOR_END_STATEMENT: s.append("end "); return;
        case RETURN_STATEMENT: s.append("return "); return;
        default: s.append(NAMES[expression.type]); return;
    }
}
