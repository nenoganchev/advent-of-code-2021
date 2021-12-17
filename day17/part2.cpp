#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>

using namespace std;

const int target_x_min = 282;
const int target_x_max = 314;
const int target_y_min = -80;
const int target_y_max = -45;
//const int target_x_min = 20;
//const int target_x_max = 30;
//const int target_y_min = -10;
//const int target_y_max = -5;

vector<int> test_y_velocity(int y_velocity);
int count_of_x_velocities_hitting_target_at_steps(const vector<int> &steps);
set<int> find_x_velocities_hitting_target_at_step(int step);


int main(int, char *[]) {
    int viable_initial_velocities = 0;
    for (int v = target_y_min; v <= abs(target_y_min); ++v) {
        vector<int> hit_steps = test_y_velocity(v);
        if (!hit_steps.empty()) {
            viable_initial_velocities += count_of_x_velocities_hitting_target_at_steps(hit_steps);
        }
    }

    cout << "Viable initial velocities: " << viable_initial_velocities << endl;
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

int count_of_x_velocities_hitting_target_at_steps(const vector<int> &steps) {
    static map<int, set<int>> velocities_cache_for_step;

    set<int> velocities;
    for (int step : steps) {
        if (!velocities_cache_for_step.contains(step)) {
            velocities_cache_for_step[step] = find_x_velocities_hitting_target_at_step(step);
        }

        velocities.insert(velocities_cache_for_step[step].begin(), velocities_cache_for_step[step].end());
    }

    return static_cast<int>(velocities.size());
}

set<int> find_x_velocities_hitting_target_at_step(int step) {
    // At steps >= 24 only vx = 24 is hitting the target [282, 314] at x = 300.
    // Lower velocities can't reach the target and higher velocities overshoot it.
    if (step >= 24) return {24};

    const int sv_min = target_x_min + ((step - 1) * step) / 2;
    const int sv_max = target_x_max + ((step - 1) * step) / 2;
    set<int> velocities;
    for (int v = target_x_min / step; step * v <= sv_max; ++v) {
        if (step * v >= sv_min) velocities.insert(v);
    }

    return velocities;
}
