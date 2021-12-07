#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include "string_utils.h"

using namespace std;

vector<int> parse_input(const string &filename);
tuple<int, int> find_optimum(const vector<int> &positions);


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: <exe> <filename>\n";
        return 1;
    }
    const string filename = argv[1];

    vector<int> positions = parse_input(filename);

    auto [optimal_position, spent_fuel] = find_optimum(positions);
    cout << "Optimal position: " << optimal_position << ", fuel: " << spent_fuel << endl;
}

vector<int> parse_input(const string &filename) {
    ifstream input_file(filename);
    vector<int> positions;

    int position;
    while (input_file >> skip(",") >> position) {
        positions.push_back(position);
    }

    return positions;
}

tuple<int, int> find_optimum(const vector<int> &positions) {
    auto minmax_pair = minmax_element(positions.cbegin(), positions.cend());
    int min_pos = *minmax_pair.first;
    int max_pos = *minmax_pair.second;

    int optimal_pos = -1;
    int optimal_fuel = numeric_limits<int>::max();
    for (int evaluated_pos = min_pos; evaluated_pos <= max_pos; ++evaluated_pos) {
        int total_fuel = 0;
        for (int pos : positions) {
            total_fuel += abs(pos - evaluated_pos);
        }

        if (total_fuel < optimal_fuel) {
            optimal_fuel = total_fuel;
            optimal_pos = evaluated_pos;
        }
    }

    return {optimal_pos, optimal_fuel};
}
