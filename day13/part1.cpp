#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "string_utils.h"

using namespace std;

struct point {
    int x;
    int y;
};

struct fold_instruction {
    enum {x, y} axis;
    int coord;
};

const int max_x = 1311;
const int max_y = 894;
int g_size_x, g_size_y;
bool g_dots[max_x][max_y];

vector<fold_instruction> parse_input(const string &filename);
void fold(fold_instruction instruction);
void fold_left(int fold_line_coord);
void fold_up(int fold_line_coord);
int count_dots();
void print_dots();


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: <exe> <filename>\n";
        return 1;
    }
    const string filename = argv[1];

    vector<fold_instruction> instructions = parse_input(filename);

    fold(instructions[0]);

    cout << "Dots visible: " << count_dots() << endl;
}

vector<fold_instruction> parse_input(const string &filename) {
    ifstream input_file(filename);
    vector<fold_instruction> instructions;

    string line;
    for (;;) {
        getline(input_file, line);
        if (line.empty()) break;

        int x, y;
        istringstream(line) >> x >> expect(",") >> y;

        g_dots[x][y] = true;
        if (g_size_x <= x) g_size_x = x + 1;
        if (g_size_y <= y) g_size_y = y + 1;
    }

    while (getline(input_file, line)) {
        char axis;
        int coord;
        istringstream(line) >> expect("fold along ") >> axis >> expect("=") >> coord;

        instructions.push_back({(axis == 'x' ? fold_instruction::x : fold_instruction::y), coord});
    }

    return instructions;
}

void fold(fold_instruction instruction) {
    if (instruction.axis == fold_instruction::x) {
        fold_left(instruction.coord);
    }
    else {
        fold_up(instruction.coord);
    }
}

void fold_left(int fold_line_coord) {
    for (int x1 = fold_line_coord - 1, x2 = fold_line_coord + 1; x1 >= 0 && x2 < g_size_x; --x1, ++x2) {
        for (int y = 0; y < g_size_y; ++y) {
            g_dots[x1][y] = g_dots[x1][y] || g_dots[x2][y];
        }
    }

    g_size_x = fold_line_coord;
}

void fold_up(int fold_line_coord) {
    for (int y1 = fold_line_coord - 1, y2 = fold_line_coord + 1; y1 >= 0 && y2 < g_size_y; --y1, ++y2) {
        for (int x = 0; x < g_size_x; ++x) {
            g_dots[x][y1] = g_dots[x][y1] || g_dots[x][y2];
        }
    }

    g_size_y = fold_line_coord;
}

int count_dots() {
    int dots = 0;

    for (int y = 0; y < g_size_y; ++y) {
        for (int x = 0; x < g_size_x; ++x) {
             if (g_dots[x][y]) ++dots;
        }
    }

    return dots;
}

void print_dots() {
    for (int y = 0; y < g_size_y; ++y) {
        for (int x = 0; x < g_size_x; ++x) {
            cout << (g_dots[x][y] ? '#' : '.');
        }
        cout << endl;
    }
}
