#include <iostream>
#include <fstream>
#include <string>

using namespace std;

const int max_size = 100;
int g_rows, g_cols;
int g_map[max_size][max_size];

void parse_input(const string &filename);
int calculate_risk_level();


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: <exe> <filename>\n";
        return 1;
    }
    const string filename = argv[1];

    parse_input(filename);

    cout << "Risk level: " << calculate_risk_level() << endl;
}

void parse_input(const string &filename) {
    ifstream input_file(filename);

    size_t i_row;
    string line;
    for (i_row = 0; getline(input_file, line); ++i_row) {
        g_cols = static_cast<int>(line.size());
        for (size_t i_col = 0; i_col < line.size(); ++i_col) {
            g_map[i_row][i_col] = line[i_col] - '0';
        }
    }

    g_rows = static_cast<int>(i_row);
}

int calculate_risk_level() {
    int risk_level = 0;

    auto height = [] (int i, int j) {
        if (i < 0 || j < 0 || i >= g_rows || j >= g_cols) return numeric_limits<int>::max();

        return g_map[i][j];
    };

    for (int i = 0; i < g_rows; ++i) {
        for (int j = 0; j < g_cols; ++j) {
            int curr_height = g_map[i][j];
            if (curr_height < height(i, j - 1) &&
                curr_height < height(i, j + 1) &&
                curr_height < height(i - 1, j) &&
                curr_height < height(i + 1, j)) {

                risk_level += curr_height + 1;
            }
        }
    }

    return risk_level;
}
