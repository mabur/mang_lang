#pragma once

#include <memory>

template<typename T>

struct Node {
    const T first;
    std::shared_ptr<Node<T>> rest;
};

template<typename T>
using SinglyLinkedList = std::shared_ptr<Node<T>>;

template<typename T>
SinglyLinkedList<T> prepend(SinglyLinkedList<T> list, T value) {
    return std::shared_ptr<Node<T>>(new Node<T>{std::move(value), std::move(list)});
}

template<typename T, typename Init, typename Operation>
Init leftFold(SinglyLinkedList<T> list, Init init, Operation operation) {
    auto result = init;
    for (auto x = list; x; x = x->rest) {
        result = operation(result, x->first);
    }
    return result;
}

template<typename T>
SinglyLinkedList<T> reverse(SinglyLinkedList<T> list) {
    return leftFold(list, SinglyLinkedList<T>{}, prepend<T>);
}

template<typename Input, typename Output>
SinglyLinkedList<Output> map(
    SinglyLinkedList<Input> list, std::function<Output(Input)> f
) {
    const auto op = [&](
        SinglyLinkedList<Output> output, Input x
    ) -> SinglyLinkedList<Output> {
        return prepend(output, f(x));
    };
    return reverse(leftFold(list, SinglyLinkedList<Output>{}, op));
}
