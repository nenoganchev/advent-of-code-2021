#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

struct coord {
    int i;
    int j;
};

const int grid_size = 10;
int g_grid[grid_size][grid_size];

void parse_input(const string &filename);
int simulate_steps();
int simulate_step();


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: <exe> <filename>\n";
        return 1;
    }
    const string filename = argv[1];

    parse_input(filename);

    cout << "Flashes after 100 steps: " << simulate_steps() << endl;
}

void parse_input(const string &filename) {
    ifstream input_file(filename);

    string line;
    for (size_t i_row = 0; i_row < grid_size; ++i_row) {
        getline(input_file, line);
        for (size_t i_col = 0; i_col < grid_size; ++i_col) {
            g_grid[i_row][i_col] = line[i_col] - '0';
        }
    }
}

int simulate_steps() {
    int flashes = 0;

    for (int i = 0; i < 100; ++i) {
        flashes += simulate_step();
    }

    return flashes;
}

int simulate_step() {
    int flashes = 0;

    vector<coord> flashing;
    for (int i = 0; i < grid_size; ++i) {
        for (int j = 0; j < grid_size; ++j) {
            if (++g_grid[i][j] > 9) {
                g_grid[i][j] = 0;
                flashing.push_back({i, j});
            }
        }
    }

    auto stimulate = [&flashing] (int i, int j) {
        if (i < 0 || j < 0 || i >= grid_size || j >= grid_size) return;
        if (g_grid[i][j] == 0) return;

        if (++g_grid[i][j] > 9) {
            g_grid[i][j] = 0;
            flashing.push_back({i, j});
        }
    };

    while (!flashing.empty()) {
        int i = flashing.back().i;
        int j = flashing.back().j;
        flashing.pop_back();
        ++flashes;

        stimulate(i - 1, j - 1);
        stimulate(i - 1, j);
        stimulate(i - 1, j + 1);
        stimulate(i, j - 1);
        stimulate(i, j + 1);
        stimulate(i + 1, j - 1);
        stimulate(i + 1, j);
        stimulate(i + 1, j + 1);
    }

    return flashes;
}
