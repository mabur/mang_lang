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

struct DictionaryNameIndexer {
    DictionaryNameIndexer() {
        dictionary_index_from_global_index = {};
    }
    ~DictionaryNameIndexer() {
        FREE_TABLE(dictionary_index_from_global_index);
    }
    size_t size() const {
        size_t count = 0;
        FOR_EACH_TABLE(it, dictionary_index_from_global_index) {
            count++;
        }
        return count;
    }
    void bindName(BoundLocalName& name) {
        name.dictionary_index = SIZE_MAX;
        GET_KEY_VALUE(name.global_index, name.dictionary_index, dictionary_index_from_global_index);
        if (name.dictionary_index != SIZE_MAX) {
            return;
        }
        name.dictionary_index = size();
        SET_KEY_VALUE(name.global_index, name.dictionary_index, dictionary_index_from_global_index);
    }
private:
    TableDictionaryIndexFromGlobalIndex dictionary_index_from_global_index;
};
