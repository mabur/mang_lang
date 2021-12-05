#include <iostream>
#include <fstream>

#include "factory.h"
#include "mang_lang.h"

namespace CommandLineArgumentIndex {
    enum {PROGRAM_PATH, INPUT_PATH, OUTPUT_PATH, LOG_PATH};
}

int main(int argc,  char **argv) {
    using namespace std;
    if (argc < CommandLineArgumentIndex::OUTPUT_PATH + 1) {
        cout << "Expected input file and output file." << endl;
        return 1;
    }

    const auto input_file_path = argv[CommandLineArgumentIndex::INPUT_PATH];
    cout << "Reading program from " << input_file_path << " ... ";
    auto input_file = ifstream{input_file_path};
    const auto code = string{(istreambuf_iterator<char>(input_file)), istreambuf_iterator<char>()};
    input_file.close();
    cout << "Done." << endl;

    try {
        cout << "Evaluating program ... ";
        const auto result = evaluate(code);
        cout << "Done." << endl;

        const auto output_file_path = argv[CommandLineArgumentIndex::OUTPUT_PATH];
        cout << "Writing result to " << output_file_path << " ... ";
        auto output_file = ofstream{output_file_path};
        output_file << result;
        output_file.close();
        cout << "Done." << endl;
    } catch (const std::runtime_error& e) {
        cout << e.what() << endl;
    }
    if (argc < CommandLineArgumentIndex::LOG_PATH + 1) {
        cout << "Skipping logging." << endl;
    } else {
        cout << "Getting log ... ";
        const auto log = getLog();
        cout << "Done." << endl;
        const auto log_file_path = argv[CommandLineArgumentIndex::LOG_PATH];
        cout << "Writing log to " << log_file_path << " ... ";
        auto log_file = ofstream{log_file_path};
        log_file << log;
        log_file.close();
        cout << "Done." << endl;
    }
}
