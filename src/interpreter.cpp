#include <chrono>
#include <iomanip>
#include <iostream>
#include <fstream>

#include "mang_lang.h"
#include "string.h"

namespace CommandLineArgumentIndex {
    enum {PROGRAM_PATH, INPUT_PATH, OUTPUT_PATH};
}

static
std::string getOutputFilePathFromInputFilePath(const std::string& input_file_path) {
    return input_file_path.substr(0, input_file_path.find_last_of('.'))
        + "_evaluated.txt";
}

int main(int argc,  char **argv) {
    using namespace std;
    if (argc < CommandLineArgumentIndex::INPUT_PATH + 1) {
        cout << "Expected input file." << endl;
        return 1;
    }
    const auto input_file_path = std::string{
        argv[CommandLineArgumentIndex::INPUT_PATH]
    };
    auto output_file_path = std::string{};
    if (argc < CommandLineArgumentIndex::OUTPUT_PATH + 1) {
        output_file_path = getOutputFilePathFromInputFilePath(input_file_path);
    } else {
        output_file_path = argv[CommandLineArgumentIndex::OUTPUT_PATH];
    }

    cout << "Reading program from " << input_file_path << " ... ";
    auto code_carma = readTextFile(input_file_path.c_str());
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
        
        cout << "Writing result to " << output_file_path << " ... ";
        auto output_file = ofstream{output_file_path};
        output_file << result;
        output_file.close();
        cout << "Done." << endl;
    } catch (const std::runtime_error& e) {
        cout << e.what() << endl;
    }
}
