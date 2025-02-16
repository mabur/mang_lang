#include "exceptions.h"

#include <stdexcept>

#include <carma/carma_string.h>

#include "expression.h"
#include "factory.h"
#include "parsing.h"

void throwException(const char* format, ...) {
    // TODO: replace implementation with carma FORMAT_STRING
    va_list args0;
    va_list args1;
    va_start(args0, format);
    va_copy(args1, args0);

    static auto string = StringBuilder{};
    CLEAR(string);
    auto num_characters = vsnprintf(string.data, (size_t)string.capacity, format, args0);
    if (num_characters >= 0) {
        if (num_characters >= (int)string.capacity) {
            RESERVE(string, (size_t) num_characters + 1);
            num_characters = vsnprintf(string.data, string.capacity, format, args1);
        }
        string.count = (size_t)num_characters;
    }

    va_end(args0);
    va_end(args1);

    throw std::runtime_error(string.data);
}

void throwUnexpectedExpressionException(ExpressionType type, const char* location) {
    throwException(
        "Unexpected expression %s for %s",
        getExpressionName(type),
        location
    );
}

void throwMissingSymbolException(const char* symbol, Expression parent) {
    throwException(
        "Cannot find symbol %s in environment of type %s",
        symbol,
        getExpressionName(parent.type)
    );
}

const char* describeLocation(CodeRange code) {
    if (code.count == 0) {
        return " at unknown location.";
    }
    else if (code.count == 1) {
        return FORMAT_STRING(
            " at row %zu and column %zu",
            firstRow(code),
            firstColumn(code)
        ).data;
    }
    else {
        return FORMAT_STRING(
            " between row %zu and column %zu and row %zu and column %zu",
            firstRow(code),
            firstColumn(code),
            lastRow(code),
            lastColumn(code)
        ).data;
    }
}
