#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <functional>
#include "string_utils.h"

using namespace std;

struct cuboid {
    int x_min, x_max;
    int y_min, y_max;
    int z_min, z_max;

    bool intersects(const cuboid &other) const;
    vector<cuboid> operator - (const cuboid &rhs) const;
};

struct reboot_step {
    cuboid region;
    bool on;
};

vector<reboot_step> parse_input(const string &filename);


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: <exe> <filename>\n";
        return 1;
    }
    const string filename = argv[1];

    vector<reboot_step> reboot_steps = parse_input(filename);

    cout << "Building intersections..." << endl;
    vector<cuboid> on_regions = build_non_intersecting_on_regions(reboot_steps);

    cout << "Number of on cubes: " << count_points_in_regions(on_regions) << endl;
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
            >> expect("x=") >> step.region.x_min >> expect("..") >> step.region.x_max >> skip(",")
            >> expect("y=") >> step.region.y_min >> expect("..") >> step.region.y_max >> skip(",")
            >> expect("z=") >> step.region.z_min >> expect("..") >> step.region.z_max;

        steps.push_back(step);
    }

    return steps;
}

vector<cuboid> build_non_intersecting_on_regions(const vector<reboot_step> &reboot_steps) {
    vector<cuboid> on_regions;

    for (const reboot_step &step : reboot_steps) {
        if (step.on) {
            vector<cuboid> regions_to_add = {step.region};

            for (const cuboid &on_region : on_regions) {
                bool changes_made;
                do {
                    changes_made = false;

                    for (auto it_region_to_add = regions_to_add.begin();
                         it_region_to_add != regions_to_add.end();
                         ++it_region_to_add) {
                        if (on_region.intersects(*it_region_to_add)) {
                            vector<cuboid> diff_regions = *it_region_to_add - on_region;

                            regions_to_add.erase(it_region_to_add);
                            regions_to_add.insert(regions_to_add.end(), diff_regions.begin(), diff_regions.end());
                            changes_made = true;

                            break;
                        }
                    }
                }
                while (changes_made);
            }

            on_regions.insert(on_regions.end(), regions_to_add.begin(), regions_to_add.end());
        }
        else {
            bool changes_made;
            do {
                changes_made = false;

                for (auto it_on_region = on_regions.begin(); it_on_region != on_regions.end(); ++it_on_region) {
                    if (step.region.intersects(*it_on_region)) {
                        vector<cuboid> diff_regions = *it_on_region - step.region;

                        on_regions.erase(it_on_region);
                        on_regions.insert(on_regions.end(), diff_regions.begin(), diff_regions.end());
                        changes_made = true;

                        break;
                    }
                }
            }
            while (changes_made);
        }
    }
}

uint64_t count_points_in_regions(const vector<cuboid> &regions) {
}
