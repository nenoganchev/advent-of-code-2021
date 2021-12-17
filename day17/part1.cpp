#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include "string_utils.h"

using namespace std;

const int target_x_min = 282;
const int target_x_max = 314;
const int target_y_min = -80;
const int target_y_max = -45;

vector<int> test_y_velocity(int y_velocity);
bool can_hit_x_in_steps(const vector<int> &steps);
int apex_for_y_velocity(int v);


int main(int, char *[]) {
    for (int v = abs(target_y_min); v > 0; --v) {
        vector<int> hit_steps = test_y_velocity(v);
        if (!hit_steps.empty() && can_hit_x_in_steps(hit_steps)) {
            cout << "Found max initial y velocity " << v << " for apex of " << apex_for_y_velocity(v) << endl;
            return 0;
        }
    }

    cout << "No suitable y velocity found" << endl;
}

vector<int> test_y_velocity(int y_velocity) {
    vector<int> hit_steps;

    int y = 0;
    int step = 0;
    while (y >= target_y_min) {
        if (y <= target_y_max) {
            hit_steps.push_back(step);
        }

        y += y_velocity--;
        ++step;
    }

    return hit_steps;
}

bool can_hit_x_in_steps(const vector<int> &steps) {
    // xmin for reaching [282, 314]: 24 (for final x = 300 at step >= 24)
    for (int step : steps) {
        if (step >= 24) return true;
    }

    return false; // haven't taken into account possible hits at steps < 24
}

int apex_for_y_velocity(int v) {
    return (v * (v + 1)) / 2;
}
