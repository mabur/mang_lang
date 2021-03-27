#pragma once

#include <string>

const std::string STANDARD_LIBRARY = R"(
{
    inc = from x to add [x, 1],
    dec = from x to sub [x, 1],
    count = from list to
        if list then
            inc count rest list
        else
            0
}
)";
