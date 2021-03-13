#pragma once

template<typename Container, typename Init, typename Operation>
Init leftFold(const Container& container, Init init, Operation operation) {
    for (const auto& element : container) {
        init = operation(init, element);
    }
    return init;
}

template<typename ValueType, typename Function>
auto map(const std::vector<ValueType>& v, Function f) {
    using NewValueType = decltype(f(v.front()));
    auto result = std::vector<NewValueType>{};
    result.reserve(v.size());
    for (const auto& x : v){
        result.push_back(f(x));
    }
    return result;
}

template<typename Container, typename Predicate>
typename Container::const_iterator findIf(
    const Container& container, Predicate predicate
) {
    auto it = container.begin();
    for (; it != container.end(); ++it) {
        if (predicate(*it)) {
            return it;
        }
    }
    return it;
}
