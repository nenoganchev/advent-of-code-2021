#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

vector<int> parse_input(const string &filename);
int calculate_sliding_window_increases(const vector<int> &measurements);


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: <exe> <filename>\n";
        return 1;
    }
    const string filename = argv[1];

    vector<int> measurements = parse_input(filename);

    cout << "Increases: " << calculate_sliding_window_increases(measurements) << endl;
}

vector<int> parse_input(const string &filename) {
    ifstream input_file(filename);
    vector<int> result;

    int n;
    while (input_file >> n) {
        result.push_back(n);
    }

    return result;
}

int calculate_sliding_window_increases(const vector<int> &measurements) {
    const size_t sliding_window_size = 3;
    const size_t sliding_windows = measurements.size() - (sliding_window_size - 1);
    auto sliding_window_sum = [&] (size_t i) {
        return measurements[i] + measurements[i + 1] + measurements[i + 2];
    };

    int increases = 0;
    for (size_t i = 1; i < sliding_windows; ++i) {
        if (sliding_window_sum(i) > sliding_window_sum(i - 1)) {
            ++increases;
        }
    }

    return increases;
}
