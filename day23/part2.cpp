#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <functional>
#include "string_utils.h"

using std::cout, std::endl;

class state {
public:
    state(const state &) = delete;
    state &operator = (const state &) = delete;

    state(const string &filename = "");
    state(state &&rhs) noexcept;
    state &operator = (state &&rhs);

    int energy() const;
    vector<state> generate_child_states() const;
    bool is_end_state() const;
    void print() const;

private:
    enum location {roomA, roomB, roomC, roomD, hallway};

    struct amphipod {
        location destination;
        location location;
        int position;
        const state *p_state;

        bool operator == (const amphipod &rhs) const;

        bool has_free_access_to_hallway() const;
        bool has_free_access_to_destination() const;
        int energy_per_distance_travelled() const;
    };

    inline static const std::set<int> valid_hallway_positions = {0, 1, 3, 5, 7, 9, 10};
    inline static const std::map<location, int> room_entry_positions = {
                                                                           {roomA, 2},
                                                                           {roomB, 4},
                                                                           {roomC, 6},
                                                                           {roomD, 8},
                                                                       };

    std::vector<amphipod> pods;
    int _energy;

    void add_part2_pods();
    void each_reachable_location(const amphipod &pod, const std::function<void(location, int, int)> &block) const;
    bool room_has_foreign_pods(location location) const;
    bool occupied(location location, int position) const;

    static state copy_without_pod(const state &source, const amphipod &removed_pod);
    static state copy_adding_pod(const state &source, const amphipod &added_pod);
};

int find_min_energy_to_end_state(const state &curr_state, int curr_min_energy = std::numeric_limits<int>::max());


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: <exe> <input type>\n";
        return 1;
    }
    const string env_type = argv[1];

    state begin_state(env_type);

    cout << "Loaded state:" << endl;
    begin_state.print();

    int min_energy_to_end_state = find_min_energy_to_end_state(begin_state);

    cout << "Optimal end state energy: " << min_energy_to_end_state << endl;
}

int find_min_energy_to_end_state(const state &curr_state, int curr_min_energy) {
    if (curr_state.is_end_state()) {
        return std::min(curr_min_energy, curr_state.energy());
    }
    if (curr_state.energy() >= curr_min_energy) return curr_min_energy;

    std::vector<state> child_states = curr_state.generate_child_states();

    for (const state &child_state : child_states) {
        int branch_min_energy = find_min_energy_to_end_state(child_state, curr_min_energy);
        curr_min_energy = std::min(curr_min_energy, branch_min_energy);
    }

    return curr_min_energy;
}

state::state(const string &filename): _energy(0) {
    if (filename.empty()) return;

    auto process_tile = [&](location tile_loc, int tile_pos, int tile_char) {
        switch (tile_char) {
        case 'A':
            pods.push_back({roomA, tile_loc, tile_pos, this});
            break;
        case 'B':
            pods.push_back({roomB, tile_loc, tile_pos, this});
            break;
        case 'C':
            pods.push_back({roomC, tile_loc, tile_pos, this});
            break;
        case 'D':
            pods.push_back({roomD, tile_loc, tile_pos, this});
            break;
        case '.':
            break;
        default:
            throw runtime_error("Invalid character in diagram");
        }
    };

    std::ifstream input_file(filename);

    expect_line(input_file, "#############");
    input_file >> expect("#");
    for (int i = 0; i < 11; ++i) {
        process_tile(hallway, i, input_file.get());
    }
    input_file >> expect("#\n")
               >> expect("###");
    for (location room : {roomA, roomB, roomC, roomD}) {
        process_tile(room, 0, input_file.get());
        input_file >> expect("#");
    }
    input_file >> expect("##\n")
               >> expect("  #");
    for (location room : {roomA, roomB, roomC, roomD}) {
        process_tile(room, 3, input_file.get());
        input_file >> expect("#");
    }
    input_file >> expect("\n");
    expect_line(input_file, "  #########");

    add_part2_pods();
}

state::state(state &&rhs) noexcept : pods(move(rhs.pods)), _energy(rhs._energy) {
    for (amphipod &pod : pods) {
        pod.p_state = this;
    }
}

state &state::operator = (state &&rhs) {
    if (this == &rhs) return *this;

    pods = std::move(rhs.pods);
    for (amphipod &pod : pods) {
        pod.p_state = this;
    }
    _energy = rhs._energy;

    return *this;
}

int state::energy() const {
    return _energy;
}

std::vector<state> state::generate_child_states() const {
    std::vector<state> child_states;

    for (const amphipod &pod : pods) {
        state base_child_state = copy_without_pod(*this, pod);

        each_reachable_location(pod, [&](location loc, int pos, int distance) {
            amphipod moved_pod = pod;
            moved_pod.location = loc;
            moved_pod.position = pos;

            state child_state = copy_adding_pod(base_child_state, moved_pod);
            child_state._energy += distance * pod.energy_per_distance_travelled();
            child_states.push_back(std::move(child_state));
        });
    }

    return child_states;
}

bool state::is_end_state() const {
    for (const amphipod &pod : pods) {
        if (pod.location != pod.destination) return false;
    }

    return true;
}

void state::print() const {
    auto pod_to_char = [](const amphipod &pod) {
        switch (pod.destination) {
        case roomA: return 'A';
        case roomB: return 'B';
        case roomC: return 'C';
        case roomD: return 'D';
        default: throw logic_error("Invalid pod type");
        }
    };

    auto char_for_tile = [&](location location, int pos) {
        for (const amphipod &pod : pods) {
            if (pod.location == location && pod.position == pos) {
                return pod_to_char(pod);
            }
        }

        return '.';
    };

    cout << "#############" << endl
         << "#";
    for (int i = 0; i < 11; ++i) {
        cout << char_for_tile(hallway, i);
    }
    cout << "#  Energy: " << _energy << endl;

    for (int i = 0; i < 4; ++i) {
        cout << (i == 0 ? "###" : "  #");
        for (location room : {roomA, roomB, roomC, roomD}) {
            cout << char_for_tile(room, i) << '#';
        }
        cout << (i == 0 ? "##" : "") << endl;
    }

    cout << "  #########" << endl
         << endl;
}

void state::add_part2_pods() {
    pods.push_back({roomD, roomA, 1, this});
    pods.push_back({roomD, roomA, 2, this});
    pods.push_back({roomC, roomB, 1, this});
    pods.push_back({roomB, roomB, 2, this});
    pods.push_back({roomB, roomC, 1, this});
    pods.push_back({roomA, roomC, 2, this});
    pods.push_back({roomA, roomD, 1, this});
    pods.push_back({roomC, roomD, 2, this});
}

void state::each_reachable_location(const amphipod &pod, const std::function<void(location, int, int)> &block) const {
    int hallway_start_pos = pod.position;
    int distance_to_hallway_start_pos = 0;
    bool can_move_to_destination = pod.has_free_access_to_destination() && !room_has_foreign_pods(pod.destination);

    if (pod.location != hallway) {
        if (!room_has_foreign_pods(pod.location)) return;
        if (!pod.has_free_access_to_hallway()) return;

        hallway_start_pos = room_entry_positions.at(pod.location);
        distance_to_hallway_start_pos = pod.position + 1;

        // don't generate intermediate hallway moves if the pod is able to directly move to its destination
        // (optimization cuts down on the number of branches and thus the size of the explored states space)
        if (!can_move_to_destination) {
            for (int pos = hallway_start_pos - 1; pos >= 0; --pos) {
                if (occupied(hallway, pos)) break;

                if (valid_hallway_positions.contains(pos)) {
                    block(hallway, pos, distance_to_hallway_start_pos + std::abs(pos - hallway_start_pos));
                }
            }
            for (int pos = hallway_start_pos + 1; pos < 11; ++pos) {
                if (occupied(hallway, pos)) break;

                if (valid_hallway_positions.contains(pos)) {
                    block(hallway, pos, distance_to_hallway_start_pos + std::abs(pos - hallway_start_pos));
                }
            }
        }
    }

    if (!can_move_to_destination) return;

    int destination_entry_pos = room_entry_positions.at(pod.destination);
    int distance_to_destination_entry_pos = distance_to_hallway_start_pos + std::abs(hallway_start_pos - destination_entry_pos);

    int deepest_accessible_room_pos = -1;
    for (int i = 0; i < 4; ++i) {
        if (occupied(pod.destination, i)) break;

        deepest_accessible_room_pos = i;
    }
    if (deepest_accessible_room_pos != -1) {
        block(pod.destination, deepest_accessible_room_pos, distance_to_destination_entry_pos + deepest_accessible_room_pos + 1);
    }
}

bool state::room_has_foreign_pods(location location) const {
    for (const amphipod &pod : pods) {
        if (pod.location == location && pod.destination != location) return true;
    }

    return false;
}

bool state::occupied(location location, int position) const {
    for (const amphipod &pod : pods) {
        if (pod.location == location && pod.position == position) return true;
    }

    return false;
}

state state::copy_without_pod(const state &source, const amphipod &removed_pod) {
    state copy;

    for (const amphipod &pod : source.pods) {
        if (pod != removed_pod) {
            copy.pods.push_back({pod.destination, pod.location, pod.position, &copy});
        }
    }

    copy._energy = source._energy;

    return copy;
}

state state::copy_adding_pod(const state &source, const amphipod &added_pod) {
    state copy;

    for (const amphipod &pod : source.pods) {
        if (pod == added_pod) throw logic_error("Added pod already present in source state");

        copy.pods.push_back({pod.destination, pod.location, pod.position, &copy});
    }
    copy.pods.push_back({added_pod.destination, added_pod.location, added_pod.position, &copy});

    copy._energy = source._energy;

    return copy;
}

bool state::amphipod::operator == (const amphipod &rhs) const {
    return location == rhs.location && position == rhs.position;
}

bool state::amphipod::has_free_access_to_hallway() const {
    if (location == hallway) throw logic_error("has_free_access_to_hallway() called on pod already in hallway");

    for (int i = 0; i < position; ++i) {
        if (p_state->occupied(location, i)) return false;
    }

    return true;
}

bool state::amphipod::has_free_access_to_destination() const {
    if (location == destination) return true;

    int destination_entry_pos = room_entry_positions.at(destination);
    int pod_start_pos = position;

    if (location != hallway) {
        if (!has_free_access_to_hallway()) return false;

        pod_start_pos = room_entry_positions.at(location);
    }

    if (pod_start_pos < destination_entry_pos) {
        for (int pos = pod_start_pos + 1; pos <= destination_entry_pos; ++pos) {
            if (p_state->occupied(hallway, pos)) return false;
        }
    }
    else {
        for (int pos = pod_start_pos - 1; pos >= destination_entry_pos; --pos) {
            if (p_state->occupied(hallway, pos)) return false;
        }
    }

    return true;
}

int state::amphipod::energy_per_distance_travelled() const {
    switch (destination) {
    case roomA: return 1;
    case roomB: return 10;
    case roomC: return 100;
    case roomD: return 1000;
    default: throw logic_error("Invalid pod destination");
    }
}
