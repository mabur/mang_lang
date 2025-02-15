#pragma once

#include <carma/carma_table.h>

#include "expression.h"

typedef struct {
    size_t key; // Global index
    size_t value; // Dictionary index
    bool occupied;
} GlobalIndexAndDictionaryIndex;

typedef struct {
    GlobalIndexAndDictionaryIndex* data;
    size_t count;
    size_t capacity;
} TableDictionaryIndexFromGlobalIndex;

size_t countTableItems(TableDictionaryIndexFromGlobalIndex table) {
    size_t count = 0;
    FOR_EACH_TABLE(it, table) {
        count++;
    }
    return count;
}

TableDictionaryIndexFromGlobalIndex bindName(TableDictionaryIndexFromGlobalIndex dictionary_index_from_global_index, BoundLocalName& name) {
    name.dictionary_index = SIZE_MAX;
    GET_KEY_VALUE(name.global_index, name.dictionary_index, dictionary_index_from_global_index);
    if (name.dictionary_index != SIZE_MAX) {
        return dictionary_index_from_global_index;
    }
    name.dictionary_index = countTableItems(dictionary_index_from_global_index);
    SET_KEY_VALUE(name.global_index, name.dictionary_index, dictionary_index_from_global_index);
    return dictionary_index_from_global_index;
}
