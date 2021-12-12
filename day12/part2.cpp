#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include "string_utils.h"

using namespace std;

struct cave {
    string name;
    int times_visited;
    int max_visits;
};

const int max_caves = 25;

vector<cave> g_caves;
bool g_conn[max_caves][max_caves];

void parse_input(const string &filename);
size_t cave_index(const string &cave_name);
size_t find_all_paths_count();
void find_all_paths(size_t i_curr_cave, vector<string> &curr_path, set<string> &all_paths);


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

    cave cave = {.name = cave_name, .times_visited = 0, .max_visits = 1};
    if (isupper(cave_name[0]) != 0) {
        cave.max_visits = numeric_limits<decltype(cave.max_visits)>::max();
    }
    g_caves.push_back(cave);
    return g_caves.size() - 1;
}

size_t find_all_paths_count() {
    vector<string> curr_path;
    set<string> all_paths;
    size_t i_start = cave_index("start");

    for (size_t i = 0; i < g_caves.size(); ++i) {
        if (g_caves[i].max_visits == 1 && g_caves[i].name != "start" && g_caves[i].name != "end") {
            g_caves[i].max_visits = 2;
            find_all_paths(i_start, curr_path, all_paths);
            g_caves[i].max_visits = 1;
        }
    }

    return all_paths.size();
}

void find_all_paths(size_t i_curr_cave, vector<string> &curr_path, set<string> &all_paths) {
    if (g_caves[i_curr_cave].name == "end") {
        all_paths.insert(join(curr_path, ",") + ",end");
        return;
    }

    ++g_caves[i_curr_cave].times_visited;
    curr_path.push_back(g_caves[i_curr_cave].name);

    for (size_t i_to = 0; i_to < max_caves; ++i_to) {
        if (g_conn[i_curr_cave][i_to] && g_caves[i_to].times_visited < g_caves[i_to].max_visits) {
            find_all_paths(i_to, curr_path, all_paths);
        }
    }

    --g_caves[i_curr_cave].times_visited;
    curr_path.pop_back();
}
