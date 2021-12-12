#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

struct coord {
    int x;
    int y;
};

const int max_size = 100;
int g_rows, g_cols;
int g_map[max_size][max_size];

void parse_input(const string &filename);
vector<coord> find_low_points();
vector<int> calculate_basin_sizes(const vector<coord> &low_points);
int explore_basin(int x, int y);


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: <exe> <filename>\n";
        return 1;
    }
    const string filename = argv[1];

    parse_input(filename);

    vector<coord> low_points = find_low_points();
    vector<int> basin_sizes = calculate_basin_sizes(low_points);
    sort(basin_sizes.begin(), basin_sizes.end(), greater<int>());

    int64_t result = 1;
    for (size_t i = 0; i < 3; ++i) {
        result *= basin_sizes[i];
    }

    cout << "Basin sizes score: " << result << endl;
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

int height(int x, int y) {
    if (x < 0 || y < 0 || x >= g_rows || y >= g_cols) return numeric_limits<int>::max();

    return g_map[x][y];
}

vector<coord> find_low_points() {
    vector<coord> low_points;

    for (int i = 0; i < g_rows; ++i) {
        for (int j = 0; j < g_cols; ++j) {
            int curr_height = g_map[i][j];
            if (curr_height < height(i, j - 1) &&
                curr_height < height(i, j + 1) &&
                curr_height < height(i - 1, j) &&
                curr_height < height(i + 1, j)) {

                low_points.push_back({i, j});
            }
        }
    }

    return low_points;
}

vector<int> calculate_basin_sizes(const vector<coord> &low_points) {
    vector<int> basin_sizes;

    for (coord low_point : low_points) {
        cout << "Exploring basin from low point [" << low_point.x << ", " << low_point.y << "]" << endl;
        basin_sizes.push_back(explore_basin(low_point.x, low_point.y));
        cout << endl;
    }

    return basin_sizes;
}

int explore_basin(int x, int y) {
    int basin_size = 1;
    int curr_height = g_map[x][y];
    g_map[x][y] = -curr_height;

    cout << "[" << x << "," << y << "](" << curr_height << ") ";

    int height_left = height(x - 1, y);
    if (curr_height < height_left && height_left < 9) basin_size += explore_basin(x - 1, y);
    int height_right = height(x + 1, y);
    if (curr_height < height_right && height_right < 9) basin_size += explore_basin(x + 1, y);
    int height_up = height(x, y - 1);
    if (curr_height < height_up && height_up < 9) basin_size += explore_basin(x, y - 1);
    int height_down = height(x, y + 1);
    if (curr_height < height_down && height_down < 9) basin_size += explore_basin(x, y + 1);

    return basin_size;
}
