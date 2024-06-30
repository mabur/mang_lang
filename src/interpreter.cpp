#include <chrono>
#include <iomanip>
#include <iostream>
#include <fstream>

#include <carma/carma.h>
#include <carma/carma_string.h>

#include "mang_lang.h"
#include "string.h"

namespace CommandLineArgumentIndex {
    enum {PROGRAM_PATH, INPUT_PATH, OUTPUT_PATH};
}

#define DROP_LAST_UNTIL_ITEM_INCLUSIVE(range, item) do { \
    FOR_EACH_REVERSE(it, (range)) { \
        DROP_LAST(range); \
        if (*it == (item)) { \
            break; \
        } \
    } \
} while(0)

static
DynamicString getOutputFilePathFromInputFilePath(ConstantString input_file_path) {
    auto result = DynamicString{};
    CONCAT(result, input_file_path);
    DROP_LAST_UNTIL_ITEM_INCLUSIVE(result, '.');
    CONCAT(result, makeStaticString("_evaluated.txt"));
    APPEND(result, '\0');
    return result;
}

int main(int argc,  char **argv) {
    using namespace std;
    if (argc < CommandLineArgumentIndex::INPUT_PATH + 1) {
        cout << "Expected input file." << endl;
        return 1;
    }
    const auto input_file_path = makeStaticString(
        argv[CommandLineArgumentIndex::INPUT_PATH]
    );
    auto output_file_path = DynamicString{};
    if (argc < CommandLineArgumentIndex::OUTPUT_PATH + 1) {
        output_file_path = getOutputFilePathFromInputFilePath(input_file_path);
    } else {
        output_file_path = makeDynamicString(argv[CommandLineArgumentIndex::OUTPUT_PATH]);
    }

    cout << "Reading program from " << input_file_path.data << " ... ";
    auto code_carma = readTextFile(input_file_path.data);
    const auto code = std::string(code_carma.data);
    FREE_DARRAY(code_carma);
    cout << "Done." << endl;

    try {
        cout << "Evaluating program ... ";
        const auto start = std::chrono::steady_clock::now();
        const auto result = evaluate_all(code);
        const auto end = std::chrono::steady_clock::now();
        const auto duration_total = std::chrono::duration<double>{end - start};
        cout << "Done. " << std::fixed << std::setprecision(1)
            << duration_total.count() << " seconds." << endl;
        
        cout << "Writing result to " << output_file_path.data << " ... ";
        auto output_file = ofstream{output_file_path.data};
        output_file << result;
        output_file.close();
        cout << "Done." << endl;
    } catch (const std::runtime_error& e) {
        cout << e.what() << endl;
    }
}
