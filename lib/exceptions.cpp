#include "exceptions.h"

#include <carma/carma_string.h>

#include "expression.h"
#include "factory.h"

String256 describeLocation(CodeRange code) {
    if (code.count == 0) {
        return formatString256(
            "It happened at row %zu and column %zu.",
            firstRow(code),
            firstColumn(code)
        );
    }
    else if (code.count == 1) {
        return formatString256(
            "It happened at row %zu and column %zu.",
            firstRow(code),
            firstColumn(code)
        );
    }
    else {
        return formatString256(
            "It happened between row %zu and column %zu and row %zu and column %zu.",
            firstRow(code),
            firstColumn(code),
            lastRow(code),
            lastColumn(code)
        );
    }
}
