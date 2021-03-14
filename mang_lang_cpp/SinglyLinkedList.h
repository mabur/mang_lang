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
Init leftFold(SinglyLinkedList<T> list, Init value, Operation operation) {
    for (; list; list = list->rest) {
        value = operation(value, list->first);
    }
    return value;
}

template<typename T>
SinglyLinkedList<T> reverse(SinglyLinkedList<T> list) {
    return leftFold(list, SinglyLinkedList<T>{}, prepend<T>);
}

template<typename Input, typename Function>
auto map(SinglyLinkedList<Input> list, Function f) {
    using Output = decltype(f(list->first));
    const auto op = [&](
        SinglyLinkedList<Output> output, Input x
    ) -> SinglyLinkedList<Output> {
        return prepend(output, f(x));
    };
    return reverse(leftFold(list, SinglyLinkedList<Output>{}, op));
}

template<typename T, typename Predicate>
SinglyLinkedList<T> findIf(SinglyLinkedList<T> list, Predicate predicate) {
    for (; list; list = list->rest) {
        if (predicate(list->first)) {
            break;
        }
    }
    return list;
}
