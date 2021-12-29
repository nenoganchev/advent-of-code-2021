#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

enum location {roomA, roomB, roomC, roomD, hallway};

struct amphipod {
    location destination;
    location location;
    int position;
};

vector<amphipod> load_env(const string &env_type);
void print(const vector<amphipod> &pods);


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: <exe> <input type>\n";
        return 1;
    }
    const string env_type = argv[1];

    vector<amphipod> pods = load_env(env_type);

    cout << "Loaded env:" << endl;
    print(pods);
}

vector<amphipod> load_env(const string &env_type) {
    vector<amphipod> pods;

    if (env_type == "full") {
        pods.push_back({roomD, roomA, 0});
        pods.push_back({roomB, roomA, 1});
        pods.push_back({roomC, roomB, 0});
        pods.push_back({roomA, roomB, 1});
        pods.push_back({roomD, roomC, 0});
        pods.push_back({roomA, roomC, 1});
        pods.push_back({roomB, roomD, 0});
        pods.push_back({roomC, roomD, 1});
    }

    return pods;
}

void print(const vector<amphipod> &pods) {
    auto pod_to_char = [](const amphipod &pod) {
        switch (pod.destination) {
        case roomA: return 'A';
        case roomB: return 'B';
        case roomC: return 'C';
        case roomD: return 'D';
        default: throw runtime_error("Invalid pod type");
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
    cout << "#" << endl;

    cout << "###";
    for (location room : {roomA, roomB, roomC, roomD}) {
        cout << char_for_tile(room, 0) << '#';
    }
    cout << "##" << endl;

    cout << "  #";
    for (location room : {roomA, roomB, roomC, roomD}) {
        cout << char_for_tile(room, 1) << '#';
    }
    cout << endl;

    cout << "  #########" << endl;
}
