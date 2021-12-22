#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <bitset>
#include <functional>
#include "string_utils.h"

using namespace std;

struct reboot_step {
    int x_min, x_max;
    int y_min, y_max;
    int z_min, z_max;
    bool on;
};

class reactor_core {
public:
    static const int min_dim = -50;
    static const int max_dim = 50;

    void turn_cube_on(int x, int y, int z);
    void turn_cube_off(int x, int y, int z);
    int report_cubes_on() const;

private:
    static const int dim_width = max_dim - min_dim + 1;

    bitset<dim_width * dim_width * dim_width> cubes;

    void set_cube(int x, int y, int z, bool on);
};

typedef void (reactor_core::*reactor_core_action)(int, int, int);

vector<reboot_step> parse_input(const string &filename);
void reboot_core(reactor_core &core, const vector<reboot_step> &reboot_steps);


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: <exe> <filename>\n";
        return 1;
    }
    const string filename = argv[1];

    vector<reboot_step> reboot_steps = parse_input(filename);

    reactor_core core;
    reboot_core(core, reboot_steps);

    cout << "Cube on after reboot: " << core.report_cubes_on() << endl;
}

vector<reboot_step> parse_input(const string &filename) {
    ifstream input_file(filename);
    vector<reboot_step> steps;

    string line;
    while (getline(input_file, line)) {
        istringstream iss(line);
        reboot_step step;

        string action;
        iss >> action;
        if (action != "on" && action != "off") throw runtime_error("Invalid reboot step action");
        step.on = action == "on";

        iss >> skip(" ")
            >> expect("x=") >> step.x_min >> expect("..") >> step.x_max >> skip(",")
            >> expect("y=") >> step.y_min >> expect("..") >> step.y_max >> skip(",")
            >> expect("z=") >> step.z_min >> expect("..") >> step.z_max;

        steps.push_back(step);
    }

    return steps;
}

void reboot_core(reactor_core &core, const vector<reboot_step> &reboot_steps) {
    auto outside_core_limits = [] (const reboot_step &step) {
        return step.x_max < reactor_core::min_dim || step.x_min > reactor_core::max_dim ||
               step.y_max < reactor_core::min_dim || step.y_min > reactor_core::max_dim ||
               step.z_max < reactor_core::min_dim || step.z_min > reactor_core::max_dim;
    };

    for (const reboot_step &step : reboot_steps) {
        if (outside_core_limits(step)) continue;

        reactor_core_action core_action = step.on ? &reactor_core::turn_cube_on : &reactor_core::turn_cube_off;
        for (int x = step.x_min; x <= step.x_max; ++x) {
            for (int y = step.y_min; y <= step.y_max; ++y) {
                for (int z = step.z_min; z <= step.z_max; ++z) {
                    invoke(core_action, core, x, y, z);
                }
            }
        }
    }
}

void reactor_core::turn_cube_on(int x, int y, int z) {
    this->set_cube(x, y, z, true);
}

void reactor_core::turn_cube_off(int x, int y, int z) {
    this->set_cube(x, y, z, false);
}

int reactor_core::report_cubes_on() const {
    return static_cast<int>(this->cubes.count());
}

void reactor_core::set_cube(int x, int y, int z, bool on) {
    if (!(min_dim <= x && x <= max_dim) || !(min_dim <= y && y <= max_dim) || !(min_dim <= z && z <= max_dim)) return;

    int norm_x = x - min_dim;
    int norm_y = y - min_dim;
    int norm_z = z - min_dim;

    int buffer_offset = norm_z + (norm_y * dim_width) + (norm_x * dim_width * dim_width);
    this->cubes[static_cast<size_t>(buffer_offset)] = on;
}
