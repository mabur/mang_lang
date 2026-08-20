#include "resolve.h"

#include <carma/carma.h>
#include <carma/carma_table.h>

#include "../expression.h"
#include "../factory.h"

namespace {

typedef struct GlobalIndexAndDictionaryIndex {
    size_t key; // Global index
    size_t value; // Dictionary index
    bool occupied;
} GlobalIndexAndDictionaryIndex;

typedef struct DictionaryIndexTable {
    GlobalIndexAndDictionaryIndex* data;
    size_t count;
    size_t capacity;
} DictionaryIndexTable;

DictionaryIndexTable bindName(DictionaryIndexTable index_table, BoundLocalName& name) {
    name.dictionary_index = SIZE_MAX;
    GET_KEY_VALUE(name.global_index, name.dictionary_index, index_table);
    if (name.dictionary_index == SIZE_MAX) {
        name.dictionary_index = index_table.count;
        SET_KEY_VALUE(name.global_index, name.dictionary_index, index_table);
    }
    return index_table;
}

void bindDictionaryNames(Dictionary& dictionary_struct) {
    auto index_table = DictionaryIndexTable{};

    FOR_EACH(i, dictionary_struct.statements) {
        const auto statement = storage.statements.data[i];
        const auto type = statement.type;
        if (type == DEFINITION) {
            auto& definition = storage.definitions.data[statement.index];
            index_table = bindName(index_table, definition.name);
        }
        else if (type == PUT_ASSIGNMENT) {
            auto& put_assignment = storage.put_assignments.data[statement.index];
            index_table = bindName(index_table, put_assignment.name);
        }
        else if (type == PUT_EACH_ASSIGNMENT) {
            auto& put_each_assignment = storage.put_each_assignments.data[statement.index];
            index_table = bindName(index_table, put_each_assignment.name);
        }
        else if (type == DROP_ASSIGNMENT) {
            auto& drop_assignment = storage.drop_assignments.data[statement.index];
            index_table = bindName(index_table, drop_assignment.name);
        }
        else if (type == FOR_STATEMENT) {
            auto& for_statement = storage.for_statements.data[statement.index];
            index_table = bindName(index_table, for_statement.item_name);
            index_table = bindName(index_table, for_statement.container_name);
        }
        else if (type == FOR_SIMPLE_STATEMENT) {
            auto& for_statement = storage.for_simple_statements.data[statement.index];
            index_table = bindName(index_table, for_statement.container_name);
        }
    }
    dictionary_struct.definition_count = index_table.count;
    FREE_TABLE(index_table);
}

} // namespace

Expression resolve(Expression expression) {
    FOR_EACH(dictionary, storage.dictionaries) {
        bindDictionaryNames(*dictionary);
    }
    return expression;
}
