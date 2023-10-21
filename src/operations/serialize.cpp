#include "serialize.h"

#include <limits>
#include <sstream>

#include "../factory.h"

namespace {

void serializeName(std::string& s, Expression name) {
    s.append(getName(name));
}

void serializeArgument(std::string& s, Expression argument) {
    const auto a = getArgument(argument);
    if (a.type.type != ANY) {
        serialize(s, a.type);
        s.append(":");
        serialize(s, a.name);
        return;
    }
    serialize(s, a.name);
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
        const auto alternative = getAlternative(a);
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
        const auto alternative = getAlternative(a);
        serialize(s, alternative.left);
        s.append(" then ");
        serialize(s, alternative.right);
        s.append(" ");
    }
    s.append("else ");
    serialize(s, is_expression.expression_else);
}

void serializeDefinition(std::string& s, const Definition& element) {
    s.append(getName(element.name));
    s.append("=");
    serialize(s, element.expression);
    s.append(" ");
}

void serializePutAssignment(std::string& s, const PutAssignment& element) {
    s.append(getName(element.name));
    s.append("+=");
    serialize(s, element.expression);
    s.append(" ");
}

void serializePutEachAssignment(std::string& s, const PutEachAssignment& element) {
    s.append(getName(element.name));
    s.append("++=");
    serialize(s, element.expression);
    s.append(" ");
}

void serializeDropAssignment(std::string& s, const DropAssignment& element) {
    s.append(getName(element.name));
    s.append("-- ");
}

void serializeWhileStatement(std::string& s, const WhileStatement& element) {
    s.append("while ");
    serialize(s, element.expression);
    s.append(" ");
}

void serializeForStatement(std::string& s, const ForStatement& element) {
    s.append("for ");
    serialize(s, element.name_item);
    s.append(" in ");
    serialize(s, element.name_container);
    s.append(" ");
}

void serializeForSimpleStatement(std::string& s, const ForSimpleStatement& element) {
    s.append("for ");
    serialize(s, element.name_container);
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
        s.append(getName(pair.name));
        s.append("=");
        serializer(s, pair.expression);
        s.append(" ");
    }
    s.back() = '}';
}

template<typename Serializer>
void serializeEvaluatedTuple(std::string& s, Serializer serializer, Expression t) {
    const auto expressions = getEvaluatedTuple(t).expressions;
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
    serializeName(s, lookup_symbol.name);
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
    const auto expressions = getTuple(t).expressions;
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
        const auto stack = getStack(expression);
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
    serializeArgument(s, function.input_name);
    s.append(" out ");
    serialize(s, function.body);
}

void serializeFunctionDictionary(std::string& s, const FunctionDictionary& function_dictionary) {
    s.append("in ");
    s.append("{");
    for (const auto& name : function_dictionary.input_names) {
        serializeArgument(s, name);
        s.append(" ");
    }
    if (function_dictionary.input_names.empty()) {
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
    for (const auto& name : function_stack.input_names) {
        serializeArgument(s, name);
        s.append(" ");
    }
    if (function_stack.input_names.empty()) {
        s.append(")");
    }
    else {
        s.back() = ')';
    }
    s.append(" out ");
    serialize(s, function_stack.body);
}
    
void serializeTable(std::string& s, Expression t) {
    const auto rows = getTable(t).rows;
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
    const auto& rows = getEvaluatedTable(t).rows;
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
    serialize_types(s, getEvaluatedStack(e).top);
    s.append("]");
}

void serializeEvaluatedStack(std::string& s, Expression expression) {
    s.append("[");
    while (expression.type != EMPTY_STACK) {
        const auto stack = getEvaluatedStack(expression);
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
        const auto string = getString(expression);
        s.push_back(getCharacter(string.top));
        expression = string.rest;
    }
    s.append("\"");
}

} // namespace

void serialize_types(std::string& s, Expression expression) {
    switch (expression.type) {
        case EVALUATED_DICTIONARY: serializeEvaluatedDictionary(s, serialize_types, getEvaluatedDictionary(expression)); return;
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
        case CONDITIONAL: serializeConditional(s, getConditional(expression)); return;
        case IS: serializeIs(s, getIs(expression)); return;
        case DICTIONARY: serializeDictionary(s, getDictionary(expression)); return;
        case EVALUATED_DICTIONARY: serializeEvaluatedDictionary(s, serialize, getEvaluatedDictionary(expression)); return;
        case DEFINITION: serializeDefinition(s, getDefinition(expression)); return;
        case PUT_ASSIGNMENT: serializePutAssignment(s, getPutAssignment(expression)); return;
        case PUT_EACH_ASSIGNMENT: serializePutEachAssignment(s, getPutEachAssignment(expression)); return;
        case DROP_ASSIGNMENT: serializeDropAssignment(s, getDropAssignment(expression)); return;
        case WHILE_STATEMENT: serializeWhileStatement(s, getWhileStatement(expression)); return;
        case FOR_STATEMENT: serializeForStatement(s, getForStatement(expression)); return;
        case FOR_SIMPLE_STATEMENT: serializeForSimpleStatement(s, getForSimpleStatement(expression)); return;
        case FUNCTION: serializeFunction(s, getFunction(expression)); return;
        case FUNCTION_DICTIONARY: serializeFunctionDictionary(s, getFunctionDictionary(expression)); return;
        case FUNCTION_TUPLE: serializeFunctionTuple(s, getFunctionTuple(expression)); return;
        case TABLE: serializeTable(s, expression); return;
        case EVALUATED_TABLE: serializeEvaluatedTable(s, getEvaluatedTable(expression).rows); return;
        case EVALUATED_TABLE_VIEW: serializeEvaluatedTable(s, getEvaluatedTableView(expression)); return;
        case TUPLE: serializeTuple(s, expression); return;
        case EVALUATED_TUPLE: serializeEvaluatedTuple(s, serialize, expression); return;
        case STACK: serializeStack(s, expression); return;
        case EVALUATED_STACK: serializeEvaluatedStack(s, expression); return;
        case LOOKUP_CHILD: serializeLookupChild(s, getLookupChild(expression)); return;
        case FUNCTION_APPLICATION: serializeFunctionApplication(s, getFunctionApplication(expression)); return;
        case LOOKUP_SYMBOL: serializeLookupSymbol(s, getLookupSymbol(expression)); return;
        case NAME: serializeName(s, expression); return;
        case ARGUMENT: serializeArgument(s, expression); return;
        case NUMBER: serializeNumber(s, getNumber(expression)); return;
        case EMPTY_STRING: serializeString(s, expression); return;
        case STRING: serializeString(s, expression); return;
        case DYNAMIC_EXPRESSION: serializeDynamicExpression(s, getDynamicExpression(expression)); return;
        case TYPED_EXPRESSION: serializeTypedExpression(s, getTypedExpression(expression)); return;
        case EMPTY_STACK: s.append("[]"); return;
        case YES: s.append("yes"); return;
        case NO: s.append("no"); return;
        case WHILE_END_STATEMENT: s.append("end "); return;
        case FOR_END_STATEMENT: s.append("end "); return;
        case RETURN_STATEMENT: s.append("return "); return;
        default: s.append(NAMES[expression.type]); return;
    }
}
