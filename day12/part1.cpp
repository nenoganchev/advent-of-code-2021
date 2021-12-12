#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "string_utils.h"

using namespace std;

struct cave {
    string name;
    bool big;
    bool explored;
};

const int max_caves = 25;

vector<cave> g_caves;
bool g_conn[max_caves][max_caves];

void parse_input(const string &filename);
size_t cave_index(const string &cave_name);
int find_all_paths_count();
void find_all_paths(size_t i_curr_cave, int &paths_count);


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: <exe> <filename>\n";
        return 1;
    }
    const string filename = argv[1];

    parse_input(filename);

    cout << "All routes: " << find_all_paths_count() << endl;
}

void parse_input(const string &filename) {
    ifstream input_file(filename);

    string line;
    while (getline(input_file, line)) {
        size_t sep_pos = line.find('-');
        string from = line.substr(0, sep_pos);
        string to = line.substr(sep_pos + 1);

        size_t i_from = cave_index(from);
        size_t i_to = cave_index(to);

        g_conn[i_from][i_to] = g_conn[i_to][i_from] = true;
    }
}

size_t cave_index(const string &cave_name) {
    for (size_t i = 0; i < g_caves.size(); ++i) {
        if (g_caves[i].name == cave_name) return i;
    }

    g_caves.push_back({.name = cave_name, .big = isupper(cave_name[0]) != 0, .explored = false});
    return g_caves.size() - 1;
}

int find_all_paths_count() {
    int n_paths = 0;
    size_t i_start = cave_index("start");

    find_all_paths(i_start, n_paths);

    return n_paths;
}

void find_all_paths(size_t i_curr_cave, int &paths_count) {
    if (g_caves[i_curr_cave].name == "end") {
        ++paths_count;
        return;
    }

    if (!g_caves[i_curr_cave].big) g_caves[i_curr_cave].explored = true;

    for (size_t i_to = 0; i_to < max_caves; ++i_to) {
        if (g_conn[i_curr_cave][i_to] && !g_caves[i_to].explored) {
            find_all_paths(i_to, paths_count);
        }
    }

    g_caves[i_curr_cave].explored = false;
}
