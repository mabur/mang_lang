#include <chrono>
#include <stdio.h>
#include <stdlib.h>

#include <carma/carma.h>
#include <carma/carma_string.h>

#include "mang_lang.h"
#include "string.h"

namespace CommandLineArgumentIndex {
    enum {PROGRAM_PATH, INPUT_PATH, OUTPUT_PATH};
}

StringView parseInputFilePath(int argc,  char **argv) {
    return STRING_VIEW(argv[CommandLineArgumentIndex::INPUT_PATH]);
}

StringBuilder parseOutputFilePath(int argc,  char **argv) {
    auto result = StringBuilder{};
    if (argc >= CommandLineArgumentIndex::OUTPUT_PATH + 1) {
        CONCAT_CSTRING(result, argv[CommandLineArgumentIndex::OUTPUT_PATH]);
    }
    else {
        CONCAT_CSTRING(result, argv[CommandLineArgumentIndex::INPUT_PATH]);
        DROP_BACK_UNTIL_ITEM(result, '.');
        DROP_BACK(result);
        CONCAT_CSTRING(result, "_evaluated.txt");
    }
    APPEND(result, '\0');
    return result;
}

int main(int argc,  char **argv) {
    using namespace std;
    if (argc < CommandLineArgumentIndex::INPUT_PATH + 1) {
        printf("Expected input file.\n");
        return 1;
    }
    auto input_file_path = parseInputFilePath(argc, argv);
    auto output_file_path = parseOutputFilePath(argc, argv);
    
    printf("Reading program from %s ... ",  input_file_path.data);
    auto code = read_text_file(input_file_path.data);
    printf("Done.\n");

    try {
        printf("Evaluating program ... ");
        const auto start = std::chrono::steady_clock::now();
        const auto result = evaluate_all(code.data);
        const auto end = std::chrono::steady_clock::now();
        const auto duration_total = std::chrono::duration<double>{end - start};
        printf("Done in %.1f seconds.\n", duration_total.count());
        
        printf("Writing result to %s ... ", output_file_path.data);
        FILE *output_file = fopen(output_file_path.data, "w");
        if (output_file != NULL) {
            fprintf(output_file, "%s", result.data);
            if (fclose(output_file) == 0) {
                printf("Done.\n");
            }
            else {
                perror("Error closing file");
                exit(EXIT_FAILURE);
            }
        }
        else {
            perror("Error opening file");
            exit(EXIT_FAILURE);
        }
    } catch (const std::runtime_error& e) {
        printf("%s\n", e.what());
    }
}
