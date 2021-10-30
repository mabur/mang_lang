#include <iostream>
#include <fstream>

#include "factory.h"
#include "mang_lang.h"

int main(int argc,  char **argv) {
    using namespace std;
    if (argc < 4) {
        cout << "Expected input file and output file and log file." << endl;
        return 1;
    }
    const auto input_file_path = argv[1];
    const auto output_file_path = argv[2];
    const auto log_file_path = argv[3];

    cout << "Reading program from " << input_file_path << " ... ";
    auto input_file = ifstream{input_file_path};
    const auto code = string{(istreambuf_iterator<char>(input_file)), istreambuf_iterator<char>()};
    input_file.close();
    cout << "Done." << endl;

    cout << "Evaluating program ... ";
    const auto result = evaluate(code);
    cout << "Done." << endl;

    cout << "Getting log ... ";
    const auto log = getLog();
    cout << "Done." << endl;

    cout << "Writing result to " << output_file_path << " ... ";
    auto output_file = ofstream{output_file_path};
    output_file << result;
    output_file.close();
    cout << "Done." << endl;

    cout << "Writing log to " << log_file_path << " ... ";
    auto log_file = ofstream{log_file_path};
    log_file << log;
    log_file.close();
    cout << "Done." << endl;
}
