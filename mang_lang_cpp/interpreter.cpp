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
    cout << "Reading program from " << input_file_path << " ... ";
    auto input_file = ifstream{input_file_path};
    const auto code = string{(istreambuf_iterator<char>(input_file)), istreambuf_iterator<char>()};
    input_file.close();
    cout << "Done." << endl;
    cout << "Evaluating program ... ";
    const auto result = evaluate(code);
    cout << "Done." << endl;
    cout << "Writing result to " << output_file_path << " ... ";
    auto output_file = ofstream{output_file_path};
    output_file << result;
    output_file.close();
    cout << "Done." << endl;
}
