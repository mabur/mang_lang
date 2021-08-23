#include <iostream>
#include <fstream>

#include "mang_lang.h"

int main(int argc,  char **argv) {
    using namespace std;
    if (argc < 3) {
        cout << "Expected input file and output file." << endl;
        return 1;
    }

    const auto input_file_path = argv[1];
    const auto output_file_path = argv[2];
    cout << "Input file: " << input_file_path << endl;
    cout << "Output file: " << output_file_path << endl;

    auto input_file = ifstream{input_file_path};
    const auto code = string{(istreambuf_iterator<char>(input_file)), istreambuf_iterator<char>()};
    input_file.close();

    const auto result = evaluate(code);
    auto output_file = ofstream{output_file_path};
    output_file << result;
    output_file.close();
}
