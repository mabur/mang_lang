#include "string.h"

#include <stdio.h>
#include <stdlib.h>

ConstantString makeStaticString(const char* s) {
    return ConstantString{s, strlen(s)};
}

std::string makeStdString(DynamicString s) {
    return std::string(s.data, s.count);
}

DynamicString concatenate(DynamicString base, const char* tail) {
    CONCAT(base, makeStaticString(tail));
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
DynamicString readStringFromFile(FILE* file, size_t num_characters) {
    auto result = DynamicString{};
    auto capacity = num_characters + 1;
    INIT_DARRAY(result, capacity, capacity);
    if (!result.data) {
        perror("Unable to allocate memory");
        FREE_DARRAY(result);
        return DynamicString{};
    }
    auto bytes_read = fread(result.data, 1, num_characters, file);
    if (bytes_read != num_characters) {
        perror("Error reading file");
        FREE_DARRAY(result);
        return DynamicString{};
    }
    result.data[bytes_read] = '\0';
    return result;
}

DynamicString readTextFile(const char* file_path) {
    auto result = DynamicString{};
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
