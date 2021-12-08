#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include "string_utils.h"

using namespace std;

struct display {
    vector<string> patterns;
    vector<string> outputs;
};

vector<display> parse_input(const string &filename);
int decode_output(const display &display);
string diff(const string &lhs, const string &rhs);
string diff(const string &lhs, char rhs);


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: <exe> <filename>\n";
        return 1;
    }
    const string filename = argv[1];

    vector<display> displays = parse_input(filename);

    int outputs_sum = 0;
    for (const display &display : displays) {
        outputs_sum += decode_output(display);
    }

    cout << "Output sums: " << outputs_sum << endl;
}

vector<display> parse_input(const string &filename) {
    ifstream input_file(filename);
    vector<display> displays;

    string line;
    while (getline(input_file, line)) {
        istringstream iss(line);
        display display;

        string token;
        vector<string> *p_storage = &display.patterns;
        while (iss >> token) {
            if (token == "|") {
                p_storage = &display.outputs;
                continue;
            }

            sort(token.begin(), token.end());
            p_storage->push_back(token);
        }

        sort(display.patterns.begin(), display.patterns.end(),
             [] (const string &lhs, const string &rhs) { return lhs.size() < rhs.size(); });

        displays.push_back(display);
    }

    return displays;
}

int decode_output(const display &display) {
    string digit_patterns[10];
    char a, c, d, f, g;  // segments

    digit_patterns[1] = display.patterns[0]; // two-digit pattern
    digit_patterns[4] = display.patterns[2]; // four-digit pattern
    digit_patterns[7] = display.patterns[1]; // three-digit pattern
    digit_patterns[8] = display.patterns[9]; // seven-digit pattern

    a = diff(digit_patterns[7], digit_patterns[1])[0];

    for (size_t i = 6; i <= 8; ++i) {  // six-digit patterns
        string curr_diff = diff(digit_patterns[1], display.patterns[i]);
        if (!curr_diff.empty()) {
            c = curr_diff[0];
            digit_patterns[6] = display.patterns[i];
            break;
        }
    }
    f = diff(digit_patterns[1], c)[0];

    string acf = {a, c, f};
    for (size_t i = 3; i <= 5; ++i) {  // five-digit patterns
        string curr_diff = diff(acf, display.patterns[i]);
        if (curr_diff.empty()) {
            digit_patterns[3] = display.patterns[i];
            break;
        }
    }

    string dg = diff(digit_patterns[3], acf);
    g = diff(dg, digit_patterns[4])[0];
    d = diff(dg, g)[0];

    for (size_t i = 6; i <= 8; ++i) {  // six-digit patterns
        if (display.patterns[i] == digit_patterns[6]) {
            continue;
        }
        else if (display.patterns[i].find(d) == string::npos) {
            digit_patterns[0] = display.patterns[i];
        }
        else {
            digit_patterns[9] = display.patterns[i];
        }
    }

    for (size_t i = 3; i <= 5; ++i) {  // five-digit patterns
        if (display.patterns[i] == digit_patterns[3]) {
            continue;
        }
        else if (display.patterns[i].find(c) == string::npos) {
            digit_patterns[5] = display.patterns[i];
        }
        else {
            digit_patterns[2] = display.patterns[i];
        }
    }

    int decoded_output = 0;
    int multiplier = 1000;
    for (const string &digit_pattern : display.outputs) {
        for (int i = 0; i < 10; ++i) {
            if (digit_pattern == digit_patterns[i]) {
                decoded_output += i * multiplier;
                multiplier /= 10;
                break;
            }
        }
    }

    return decoded_output;
}

string diff(const string &lhs, const string &rhs) {
    string result;
    for (char lhs_char : lhs) {
        if (rhs.find(lhs_char) == string::npos) {
            result.append(1, lhs_char);
        }
    }

    return result;
}

string diff(const string &lhs, char rhs) {
    return diff(lhs, string(1, rhs));
}
