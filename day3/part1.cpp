#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <bitset>
#include "string_utils.h"

using namespace std;

typedef bitset<12> diag_binary;

vector<diag_binary> parse_input(const string &filename);
unsigned long calculate_power_consumption(const vector<diag_binary> &diag_binaries);


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: <exe> <filename>\n";
        return 1;
    }
    const string filename = argv[1];

    vector<diag_binary> diag_binaries = parse_input(filename);
    cout << "Power consumption: " << calculate_power_consumption(diag_binaries) << endl;
}

vector<diag_binary> parse_input(const string &filename) {
    ifstream input_file(filename);
    vector<diag_binary> binaries;

    string line;
    while (getline(input_file, line)) {
        binaries.push_back(diag_binary(line));
    }

    return binaries;
}

unsigned long calculate_power_consumption(const vector<diag_binary> &diag_binaries) {
    diag_binary gamma_rate;
    diag_binary eps_rate;
    size_t bit_width = diag_binaries[0].size();

    for (size_t i = 0; i < bit_width; ++i) {
        int ones = 0;
        int zeroes = 0;
        for (const diag_binary &binary : diag_binaries) {
            binary[i] ? ++ones : ++zeroes;
        }

        ones > zeroes ? gamma_rate.set(i) : eps_rate.set(i);
    }

    return gamma_rate.to_ulong() * eps_rate.to_ulong();
}
