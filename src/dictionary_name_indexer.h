#pragma once

#include <unordered_map>

#include "expression.h"

struct DictionaryNameIndexer {
    size_t size() const {
        return dictionary_index_from_global_index.size();
    }
    void bindName(BoundLocalName& name) {
        name.dictionary_index = getDictionaryIndex(name.global_index);
    }
private:
    size_t getDictionaryIndex(size_t global_index) {
        const auto it = dictionary_index_from_global_index.find(global_index);
        if (it != dictionary_index_from_global_index.end()) {
            return it->second;
        }
        else {
            const auto count = size();
            dictionary_index_from_global_index[global_index] = count;
            return count;
        }
    }
    std::unordered_map<size_t, size_t> dictionary_index_from_global_index;
};
