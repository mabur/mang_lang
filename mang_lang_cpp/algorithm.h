#pragma once

template<typename Container, typename Init, typename Operation>
Init leftFold(const Container& container, Init init, Operation operation) {
    for (const auto& element : container) {
        init = operation(init, element);
    }
    return init;
}
