#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <tuple>
#include "string_utils.h"

using namespace std;

struct display {
    vector<string> patterns;
    vector<string> outputs;
};

vector<display> parse_input(const string &filename);
int count_simple_outputs(const vector<display> &displays);


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: <exe> <filename>\n";
        return 1;
    }
    const string filename = argv[1];

    vector<display> displays = parse_input(filename);

    cout << "Simple outputs: " << count_simple_outputs(displays) << endl;
}

vector<display> parse_input(const string &filename) {
    ifstream input_file(filename);
    vector<display> displays;

    string line;
    while (getline(input_file, line)) {
        istringstream iss(line);
        display display;

        string token;
        while (iss >> token) {
            if (token == "|") break;

            display.patterns.push_back(token);
        }

        while (iss >> token) {
            display.outputs.push_back(token);
        }

        displays.push_back(display);
    }

    return displays;
}

int count_simple_outputs(const vector<display> &displays) {
    int count = 0;

    for (const display &display : displays) {
        for (const string &output : display.outputs) {
            switch (output.size()) {
                case 2:
                case 3:
                case 4:
                case 7:
                    ++count;
            }
        }
    }

    return count;
}
