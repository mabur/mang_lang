#include "string.h"

#include <stdio.h>
#include <stdlib.h>

StringView makeStaticString(const char* s) {
    return StringView{s, strlen(s)}; // TODO: should it be +1? No.
}

StringBuilder makeStringBuilder(char* s) {
    auto count = strlen(s);
    return StringBuilder{s, count, count}; // TODO: should it be +1? No.
}

std::string makeStdString(StringBuilder s) {
    return std::string(s.data, s.count);
}

StringBuilder concatenate(StringBuilder base, const char* tail) {
    CONCAT_CSTRING(base, tail);
    return base;
}

static
long getFileSize(FILE* file) {
    if (fseek(file, 0, SEEK_END) != 0) {
        perror("Error seeking end of file");
        return -1;
    }
    long result = ftell(file);
    if (result == -1) {
        perror("Error getting file size");
        return -1;
    }
    if (fseek(file, 0, SEEK_SET) != 0) {
        perror("Error seeking start of file");
        return -1;
    }
    return result;
}

static
StringBuilder readStringFromFile(FILE* file, size_t num_characters) {
    auto result = StringBuilder{};
    auto capacity = num_characters + 1;
    INIT_DARRAY(result, capacity, capacity);
    if (!result.data) {
        perror("Unable to allocate memory");
        FREE_DARRAY(result);
        return StringBuilder{};
    }
    auto bytes_read = fread(result.data, 1, num_characters, file);
    if (bytes_read != num_characters) {
        perror("Error reading file");
        FREE_DARRAY(result);
        return StringBuilder{};
    }
    result.data[bytes_read] = '\0';
    return result;
}

StringBuilder readTextFile(const char* file_path) {
    auto result = StringBuilder{};
    auto file = fopen(file_path, "rb");
    if (!file) {
        perror("Unable to open file");
        return result;
    }
    long file_size = getFileSize(file);
    if (file_size > 0) {
        result = readStringFromFile(file, (size_t)file_size);
    }
    fclose(file);
    return result;
}
