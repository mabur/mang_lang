#include "serialize.h"

#include <limits>
#include <sstream>

#include "../factory.h"
#include "../built_in_functions/container.h"

namespace {

std::string serializeName(Expression name) {
    return getName(name);
}

std::string serializeArgument(Expression argument) {
    const auto a = getArgument(argument);
    if (a.type.type != ANY) {
        return serialize(a.type) + ':' + serialize(a.name);    
    }
    return serialize(a.name);
}

std::string serializeDynamicExpression(const DynamicExpression& dynamic_expression) {
    return "dynamic " + serialize(dynamic_expression.expression);
}

std::string serializeTypedExpression(const TypedExpression& typed_expression) {
    return serialize(typed_expression.type) + ':' + serialize(typed_expression.value);
}

std::string serializeConditional(const Conditional& conditional) {
    auto result = std::string{"if "};
    for (const auto& alternative : conditional.alternatives) {
        result += serialize(alternative.left);
        result += " then ";
        result += serialize(alternative.right);
        result += " ";
    }
    result += "else ";
    result += serialize(conditional.expression_else);
    return result;
}

std::string serializeIs(const IsExpression& is_expression) {
    auto result = "is " + serialize(is_expression.input) + " ";
    for (const auto& alternative : is_expression.alternatives) {
        result += serialize(alternative.left);
        result += " then ";
        result += serialize(alternative.right);
        result += " ";
    }
    result += "else ";
    result += serialize(is_expression.expression_else);
    return result;
}

std::string serializeDefinition(const Definition& element) {
    return getName(element.name) + '=' + serialize(element.expression) + ' ';
}

std::string serializePutAssignment(const PutAssignment& element) {
    return getName(element.name) + "+=" + serialize(element.expression) + ' ';
}

std::string serializeDropAssignment(const DropAssignment& element) {
    return getName(element.name) + "-- ";
}

std::string serializeWhileStatement(const WhileStatement& element) {
    return "while " + serialize(element.expression) + ' ';
}

std::string serializeForStatement(const ForStatement& element) {
    return "for " + serialize(element.name_item) + " in " +
        serialize(element.name_container) + " ";
}

template<typename Serializer>
std::string serializeEvaluatedDictionary(Serializer serializer, const EvaluatedDictionary& dictionary) {
    if (dictionary.definitions.empty()) {
        return "{}";
    }
    auto result = std::string{"{"};
    for (const auto& pair : dictionary.definitions.sorted()) {
        result += pair.first + "=" + serializer(pair.second) + " ";
    }
    result.back() = '}';
    return result;
}

template<typename Serializer>
std::string serializeEvaluatedTuple(Serializer serializer, Expression s) {
    const auto expressions = getEvaluatedTuple(s).expressions;
    if (expressions.empty()) {
        return "()";
    }
    auto result = std::string{'('};
    for (const auto& expression : expressions) {
        result += serializer(expression) + ' ';
    }
    result.back() = ')';
    return result;
}

std::string serializeLookupChild(const LookupChild& lookup_child) {
    return serializeName(lookup_child.name) + "@" + serialize(lookup_child.child);
}

std::string serializeFunctionApplication(const FunctionApplication& function_application) {
    return serializeName(function_application.name) + "!" +
        serialize(function_application.child);
}

std::string serializeLookupSymbol(const LookupSymbol& lookup_symbol) {
    return serializeName(lookup_symbol.name);
}
    
std::string serializeDictionary(const Dictionary& dictionary) {
    auto result = std::string{};
    result += '{';
    for (const auto& statement : dictionary.statements) {
        result += serialize(statement);
    }
    if (dictionary.statements.empty()) {
        result += '}';
    }
    else {
        result.back() = '}';
    }
    return result;
}

std::string serializeTuple(Expression s) {
    const auto expressions = getTuple(s).expressions;
    if (expressions.empty()) {
        return "()";
    }
    auto result = std::string{'('};
    for (const auto& expression : expressions) {
        result += serialize(expression) + ' ';
    }
    result.back() = ')';
    return result;
}

std::string serializeStack(Expression s) {
    const auto appendElement = [=](const std::string& s, Expression element) {
        return s + serialize(element) + ' ';
    };
    auto result = leftFold(
        std::string{"["}, s, appendElement, EMPTY_STACK, getStack
    );
    result.back() = ']';
    return result;
}

std::string serializeCharacter(Character character) {
    return {'\'', character, '\''};
}

std::string serializeFunction(const Function& function) {
    return "in " + serializeArgument(function.input_name)
        + " out " + serialize(function.body);
}

std::string serializeFunctionDictionary(const FunctionDictionary& function_dictionary) {
    auto result = std::string{};
    result += "in ";
    result += "{";
    for (const auto& name : function_dictionary.input_names) {
        result += serializeArgument(name);
        result += " ";
    }
    if (function_dictionary.input_names.empty()) {
        result += '}';
    }
    else {
        result.back() = '}';
    }
    result += " out " + serialize(function_dictionary.body);
    return result;
}

std::string serializeFunctionTuple(const FunctionTuple& function_stack) {
    auto result = std::string{};
    result += "in ";
    result += "(";
    for (const auto& name : function_stack.input_names) {
        result += serializeArgument(name);
        result += " ";
    }
    if (function_stack.input_names.empty()) {
        result += ')';
    }
    else {
        result.back() = ')';
    }
    result += " out " + serialize(function_stack.body);
    return result;
}
    
std::string serializeTable(Expression s) {
    const auto rows = getTable(s).rows;
    if (rows.empty()) {
        return "<>";
    }
    auto result = std::string{'<'};
    for (const auto& row : rows) {
        result += '(' + serialize(row.key) + ' ' + serialize(row.value) + ") ";
    }
    result.back() = '>';
    return result;
}

std::string serializeTypesEvaluatedTable(Expression s) {
    const auto& rows = getEvaluatedTable(s).rows;
    if (rows.empty()) {
        return "<>";
    }
    const auto& row = rows.begin()->second;
    return "<(" + serialize_types(row.key) + ' ' + serialize_types(row.value) + ")>";
}

template<typename Table>
std::string serializeEvaluatedTable(const Table& table) {
    if (table.empty()) {
        return "<>";
    }
    auto result = std::string{'<'};
    for (const auto& row : table) {
        result += '(' + row.first + ' ' + serialize(row.second.value) + ") ";
    }
    result.back() = '>';
    return result;
}

std::string serializeTypesEvaluatedStack(Expression s) {
    return '[' + serialize_types(getEvaluatedStack(s).top) + ']';
}
    
std::string appendElement(const std::string& s, Expression element) {
    return s + serialize(element) + ' ';
}

std::string serializeEvaluatedStack(Expression s) {
    auto result = leftFold(std::string{"["}, s, appendElement, EMPTY_STACK,
        getEvaluatedStack);
    result.back() = ']';
    return result;
}

std::string serializeNumber(Number number) {
    std::stringstream s;
    s.precision(std::numeric_limits<double>::digits10 + 1);
    s << number;
    return s.str();
}

std::string serializeBoolean(Boolean boolean) {
    return boolean ? "yes" : "no";
}

std::string appendCharacter(const std::string& s, Expression character) {
    return s + getCharacter(character);
}

std::string serializeString(Expression string) {
    const auto delimiter = std::string{"\""};
    return leftFold(delimiter, string, appendCharacter, EMPTY_STRING, getString)
        + delimiter;
}

} // namespace

std::string serialize_types(Expression expression) {
    switch (expression.type) {
        case EVALUATED_DICTIONARY: return serializeEvaluatedDictionary(serialize_types, getEvaluatedDictionary(expression));
        case EVALUATED_TUPLE: return serializeEvaluatedTuple(serialize_types, expression);
        case EVALUATED_STACK: return serializeTypesEvaluatedStack(expression);
        case EVALUATED_TABLE: return serializeTypesEvaluatedTable(expression);
        // TODO: EVALUATED_TABLE_VIEW?
        default: return NAMES[expression.type];
    }
}

std::string serialize(Expression expression) {
    switch (expression.type) {
        case CHARACTER: return serializeCharacter(getCharacter(expression));
        case CONDITIONAL: return serializeConditional(getConditional(expression));
        case IS: return serializeIs(getIs(expression));
        case DICTIONARY: return serializeDictionary(getDictionary(expression));
        case EVALUATED_DICTIONARY: return serializeEvaluatedDictionary(serialize, getEvaluatedDictionary(expression));
        case DEFINITION: return serializeDefinition(getDefinition(expression));
        case PUT_ASSIGNMENT: return serializePutAssignment(getPutAssignment(expression));
        case DROP_ASSIGNMENT: return serializeDropAssignment(getDropAssignment(expression));
        case WHILE_STATEMENT: return serializeWhileStatement(getWhileStatement(expression));
        case FOR_STATEMENT: return serializeForStatement(getForStatement(expression));
        case WHILE_END_STATEMENT: return "end ";
        case FOR_END_STATEMENT: return "end ";
        case FUNCTION: return serializeFunction(getFunction(expression));
        case FUNCTION_DICTIONARY: return serializeFunctionDictionary(getFunctionDictionary(expression));
        case FUNCTION_TUPLE: return serializeFunctionTuple(getFunctionTuple(expression));
        case TABLE: return serializeTable(expression);
        case EVALUATED_TABLE: return serializeEvaluatedTable(getEvaluatedTable(expression).rows);
        case EVALUATED_TABLE_VIEW: return serializeEvaluatedTable(getEvaluatedTableView(expression));
        case TUPLE: return serializeTuple(expression);
        case EVALUATED_TUPLE: return serializeEvaluatedTuple(serialize, expression);
        case STACK: return serializeStack(expression);
        case EVALUATED_STACK: return serializeEvaluatedStack(expression);
        case EMPTY_STACK: return "[]";
        case LOOKUP_CHILD: return serializeLookupChild(getLookupChild(expression));
        case FUNCTION_APPLICATION: return serializeFunctionApplication(getFunctionApplication(expression));
        case LOOKUP_SYMBOL: return serializeLookupSymbol(getLookupSymbol(expression));
        case NAME: return serializeName(expression);
        case ARGUMENT: return serializeArgument(expression);
        case NUMBER: return serializeNumber(getNumber(expression));
        case BOOLEAN: return serializeBoolean(getBoolean(expression));
        case EMPTY_STRING: return serializeString(expression);
        case STRING: return serializeString(expression);
        case DYNAMIC_EXPRESSION: return serializeDynamicExpression(getDynamicExpression(expression));
        case TYPED_EXPRESSION: return serializeTypedExpression(getTypedExpression(expression));
        default: return NAMES[expression.type];
    }
}
