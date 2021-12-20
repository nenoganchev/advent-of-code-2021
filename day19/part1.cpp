#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include "string_utils.h"

using namespace std;

struct vector_3d {
    int x;
    int y;
    int z;

    vector_3d operator - (const vector_3d &rhs) const;
    uint64_t measure() const;
};

typedef vector_3d point;
typedef map<uint64_t, vector<tuple<size_t, size_t>>> map_distances;

vector<vector<vector_3d>> parse_input(const string &filename);
vector<vector_3d> parse_scanner_readings(ifstream &is);
vector<vector_3d> build_unified_map(const vector<vector<vector_3d>> &partial_maps);
map_distances calculate_distances_between(const vector<vector_3d> &vectors);
tuple<size_t, size_t, size_t> analyze_distances(const map<uint64_t, vector<tuple<size_t, size_t>>> &set1_distances,
                                                const map<uint64_t, vector<tuple<size_t, size_t>>> &set2_distances);

template<class T, class U>
ostream &operator << (ostream &os, const tuple<T, U> &t) { os << "[" << get<0>(t) << ", " << get<1>(t) << "]"; return os; }
template<class T>
ostream &operator << (ostream &os, const vector<T> &v) { for (const T &elem : v) { os << elem << " "; } return os; }


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: <exe> <filename>\n";
        return 1;
    }
    const string filename = argv[1];

    vector<vector<vector_3d>> scanners_readings = parse_input(filename);

    vector<vector_3d> unified_map = build_unified_map(scanners_readings);

    cout << "Probes in unified map: " << unified_map.size() << endl;
}

vector<vector<vector_3d>> parse_input(const string &filename) {
    ifstream input_file(filename);
    vector<vector<vector_3d>> scanners_readings;

    while (input_file.peek() != ifstream::traits_type::eof()) {
        scanners_readings.push_back(parse_scanner_readings(input_file));
    }

    return scanners_readings;
}

vector<vector_3d> parse_scanner_readings(ifstream &is) {
    vector<vector_3d> scanner_readings;

    expect_line_starting_with(is, "--- scanner");

    string line;
    while (getline(is, line)) {
        if (line.empty()) break;

        istringstream iss(line);
        vector_3d v;
        iss >> v.x >> skip(",") >> v.y >> skip(",") >> v.z;

        scanner_readings.push_back(v);
    }

    return scanner_readings;
}

vector<vector_3d> build_unified_map(const vector<vector<point>> &partial_maps) {
    vector<vector_3d> unified_map = partial_maps[0];

    map<size_t, map_distances> unmerged_maps;
    for (size_t i = 1; i < partial_maps.size(); ++i) {
        unmerged_maps[i] = calculate_map_distances(partial_maps[i]);
    }

    while (!unmerged_maps.empty()) {
        auto it_overlapping_unmerged_map = find_overlapping_unmerged_map(unified_map, unmerged_maps);
        merge_maps(unified_map, partial_maps[it_overlapping_unmerged_map->first], it_overlapping_unmerged_map->second);
        unmerged_maps.erase(it_overlapping_unmerged_map);
    }

    return unified_map;
}

map_distances calculate_map_distances(const vector<point> &points) {
    map_distances distances;

    for (size_t i = 0; i < points.size(); ++i) {
        for (size_t j = i + 1; j < points.size(); ++j) {
            uint64_t distance = (points[i] - points[j]).measure();
            distances[distance].push_back({i, j});
        }
    }

    return distances;
}

map<size_t, map_distances>::const_iterator find_overlapping_unmerged_map(const vector<vector_3d> &unified_map,
                                                                         const map<size_t, map_distances> &unmerged_maps) {
    map_distances unified_map_distances = calculate_map_distances(unified_map);

    for (auto it_unmerged_map = unmerged_maps.cbegin(); it_unmerged_map != unmerged_maps.cend(); ++it_unmerged_map) {
        int matching_distances = 0;
        for (const auto &[dist, set1_pairs] : set1_distances) {
            auto set2_matches = set2_distances.find(dist);
            if (set2_matches != set2_distances.end()) {
                ++matching_distances;
                pairs_in_set1 += set1_pairs.size();
                pairs_in_set2 += set2_matches->second.size();
            }
        }
    }
}

vector_3d vector_3d::operator - (const vector_3d &rhs) const {
    return {x - rhs.x, y - rhs.y, z - rhs.z};
}

uint64_t vector_3d::measure() const {
    return static_cast<uint64_t>(x*x + y*y + z*z);
}

tuple<size_t, size_t, size_t> analyze_distances(const map<uint64_t, vector<tuple<size_t, size_t>>> &set1_distances,
                                                const map<uint64_t, vector<tuple<size_t, size_t>>> &set2_distances) {
    cout << "Sets with 12 apparent matches:" << endl;
    for (size_t i = 0; i < maps_distances.size(); ++i) {
        for (size_t j = i + 1; j < maps_distances.size(); ++j) {
            auto [matching_distances, pairs_in_set1, pairs_in_set2] = analyze_distances(maps_distances[i], maps_distances[j]);

            if (matching_distances == 66) {
                cout << "  " << i << " <-> " << j;
                if (matching_distances != pairs_in_set1 || matching_distances != pairs_in_set2) {
                    cout << " [WARNING: unusual number of set pairs: " << pairs_in_set1 << ", " << pairs_in_set2 << "]";
                }
                cout << endl;
            }

            //if (matching_distances == 66) {
                //cout << "Matches in " << i << " <-> " << j << " : [distances: " << matching_distances << "; set1 pairs: " << pairs_in_set1 << "; set2 pairs: " << pairs_in_set2 << "]" << endl;
            //}
        }
    }

    size_t matching_distances = 0;
    size_t pairs_in_set1 = 0;
    size_t pairs_in_set2 = 0;

    for (const auto &[dist, set1_pairs] : set1_distances) {
        auto set2_matches = set2_distances.find(dist);
        if (set2_matches != set2_distances.end()) {
            ++matching_distances;
            pairs_in_set1 += set1_pairs.size();
            pairs_in_set2 += set2_matches->second.size();
        }
    }

    return {matching_distances, pairs_in_set1, pairs_in_set2};
}

