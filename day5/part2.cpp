#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include "string_utils.h"

using namespace std;

struct point {
    int x;
    int y;
};

struct line {
    point a;
    point b;
};

const int map_size = 1000;
uint8_t g_map[map_size][map_size];

vector<line> parse_input(const string &filename);
int calculate_overlaps(const vector<line> &lines);
bool horizontal(const line &line);
bool vertical(const line &line);
bool diagonal(const line &line);


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: <exe> <filename>\n";
        return 1;
    }
    const string filename = argv[1];

    vector<line> lines = parse_input(filename);

    cout << "Overlapping points: " << calculate_overlaps(lines) << endl;
}

vector<line> parse_input(const string &filename) {
    ifstream input_file(filename);
    vector<line> lines;

    string text_line;
    while (getline(input_file, text_line)) {
        line l;
        istringstream iss(text_line);
        iss >> l.a.x >> skip(",") >> l.a.y >> expect(" -> ") >> l.b.x >> skip(",") >> l.b.y;

        lines.push_back(l);
    }

    return lines;
}

int calculate_overlaps(const vector<line> &lines) {
    for (const line &line : lines) {
        if (horizontal(line)) {
            auto [x1, x2] = minmax(line.a.x, line.b.x);
            for (int x = x1; x <= x2; ++x) {
                ++g_map[x][line.a.y];
            }
        }
        else if (vertical(line)) {
            auto [y1, y2] = minmax(line.a.y, line.b.y);
            for (int y = y1; y <= y2; ++y) {
                ++g_map[line.a.x][y];
            }
        }
        else if (diagonal(line)) {
            int n_points = abs(line.a.x - line.b.x) + 1;
            int dx = line.a.x < line.b.x ? 1 : -1;
            int dy = line.a.y < line.b.y ? 1 : -1;

            point curr_point = line.a;
            for (int i = 0; i < n_points; ++i, curr_point.x += dx, curr_point.y += dy) {
                ++g_map[curr_point.x][curr_point.y];
            }
        }
        else {
            throw runtime_error("Non-conformant line");
        }
    }

    int overlaps = 0;
    for (int x = 0; x < map_size; ++x) {
        for (int y = 0; y < map_size; ++y) {
            if (g_map[x][y] > 1) {
                ++overlaps;
            }
        }
    }

    return overlaps;
}

bool horizontal(const line &line) {
    return line.a.y == line.b.y;
}

bool vertical(const line &line) {
    return line.a.x == line.b.x;
}

bool diagonal(const line &line) {
    return abs(line.a.x - line.b.x) == abs(line.a.y - line.b.y);
}
