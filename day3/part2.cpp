#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <bitset>
#include "string_utils.h"

using namespace std;

const size_t input_size = 1000;
typedef bitset<12> diag_binary;
typedef bool (*bit_criteria_callback)(int ones, int zeroes);

vector<diag_binary> parse_input(const string &filename);
unsigned long find_rating(const vector<diag_binary> &diag_binaries, bit_criteria_callback bit_criteria);


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: <exe> <filename>\n";
        return 1;
    }
    const string filename = argv[1];

    vector<diag_binary> diag_binaries = parse_input(filename);

    unsigned long o2_rating = find_rating(
                                diag_binaries,
                                [] (int ones, int zeroes) -> bool { return ones < zeroes ? 0 : 1; }
                              );
    unsigned long co2_rating = find_rating(
                                 diag_binaries,
                                 [] (int ones, int zeroes) -> bool { return ones < zeroes ? 1 : 0; }
                               );

    cout << "Life support rating: " << o2_rating * co2_rating << endl;
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

unsigned long find_rating(const vector<diag_binary> &diag_binaries, bit_criteria_callback bit_criteria) {
    size_t bit_width = diag_binaries[0].size();
    bitset<input_size> filter;
    filter.set();

    for (size_t i_bit = 0; i_bit < bit_width; ++i_bit) {
        size_t bit_pos = bit_width - i_bit - 1;

        int ones = 0;
        int zeroes = 0;
        for (size_t i_binary = 0; i_binary < diag_binaries.size(); ++i_binary) {
            if (!filter[i_binary]) continue;

            diag_binaries[i_binary][bit_pos] ? ++ones : ++zeroes;
        }

        bool desired_bit = bit_criteria(ones, zeroes);

        for (size_t i_binary = 0; i_binary < diag_binaries.size(); ++i_binary) {
            if (filter[i_binary] && diag_binaries[i_binary][bit_pos] != desired_bit) {
                filter[i_binary] = 0;
            }
        }

        if (filter.count() == 1) {
            break;
        }
    }

    auto last_remaining_element_index = [] (const std::bitset<input_size> &filter) {
        for (size_t i = 0; i < filter.size(); ++i) {
            if (filter[i] == 1) {
                return i;
            }
        }

        throw runtime_error("Empty filter");
    };

    return diag_binaries[last_remaining_element_index(filter)].to_ulong();
}
