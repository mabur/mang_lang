#include "exceptions.h"

#include <stdexcept>

#include <carma/carma_string.h>

#include "parsing.h"

void throwException(const char* format, ...) {
    va_list args0;
    va_list args1;
    va_start(args0, format);
    va_copy(args1, args0);

    static auto string = DynamicString{};
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

static
std::string serializeCodeCharacter(const CodeCharacter* c) {
    return "row " + std::to_string(c->row + 1) + " and column "
        + std::to_string(c->column + 1);
}

std::string describeLocation(CodeRange code) {
    if (code.count == 0) {
        return " at unknown location.";
    }
    if (code.count == 1) {
        return " at " + serializeCodeCharacter(code.data);
    }
    return " between " +
        serializeCodeCharacter(code.data) + " and " +
        serializeCodeCharacter(code.data + code.count - 1);
}
