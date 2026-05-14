#include "exceptions.h"

#include <carma/carma_string.h>

#include "expression.h"
#include "factory.h"

const char* describeLocation(CodeRange code) {
    if (code.count == 0) {
        return "It happened at an unknown location.";
    }
    else if (code.count == 1) {
        return FORMAT_STRING(
            "It happened at row %zu and column %zu.",
            firstRow(code),
            firstColumn(code)
        ).data;
    }
    else {
        return FORMAT_STRING(
            "It happened between row %zu and column %zu and row %zu and column %zu.",
            firstRow(code),
            firstColumn(code),
            lastRow(code),
            lastColumn(code)
        ).data;
    }
}
