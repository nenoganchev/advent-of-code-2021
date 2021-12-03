#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <tuple>
#include "string_utils.h"

using namespace std;

struct command {
    enum type {forward, down, up};

    type type;
    int arg;
};

vector<command> parse_input(const string &filename);
tuple<int, int> calculate_position(const vector<command> &commands);


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: <exe> <filename>\n";
        return 1;
    }
    const string filename = argv[1];

    vector<command> commands = parse_input(filename);
    auto [horizontal_pos, depth] = calculate_position(commands);

    cout << "Horizontal position: " << horizontal_pos << ", depth: " << depth
         << " (result: " << horizontal_pos * depth << ")\n";
}

vector<command> parse_input(const string &filename) {
    ifstream input_file(filename);
    vector<command> result;

    string type;
    int arg;
    while (input_file >> type >> arg) {
        command command;

        if (type == "forward") {
            command.type = command::forward;
        }
        else if (type == "down") {
            command.type = command::down;
        }
        else if (type == "up") {
            command.type = command::up;
        }
        else {
            throw runtime_error("Invalid command");
        }
        command.arg = arg;

        result.push_back(command);
    }

    return result;
}

tuple<int, int> calculate_position(const vector<command> &commands) {
    int horizontal_pos = 0;
    int depth = 0;
    int aim = 0;

    for (const command &command : commands) {
        if (command.type == command::forward) {
            horizontal_pos += command.arg;
            depth += aim * command.arg;
        }
        else if (command.type == command::down) {
            aim += command.arg;
        }
        else if (command.type == command::up) {
            aim -= command.arg;
        }
    }

    return {horizontal_pos, depth};
}
