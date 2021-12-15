#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <functional>

using namespace std;

class graph {
public:
    struct vertex {
        int x;
        int y;

        bool operator == (const vertex &rhs) const = default;
    };

    graph(istream &input);
    void expand_map();

    int size_x() const;
    int size_y() const;

    void each_neighbour(vertex v, const function<void(vertex, int)> &block) const;

private:
    uint8_t map[500][500];
    int map_size_x;
    int map_size_y;
};

int shortest_path(const graph &g, graph::vertex start, graph::vertex end);


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: <exe> <filename>\n";
        return 1;
    }
    const string filename = argv[1];

    ifstream input_file(filename);
    graph g(input_file);
    cout << "Initial size:  " << g.size_x() << " " << g.size_y() << endl;
    g.expand_map();
    cout << "Expanded size: " << g.size_x() << " " << g.size_y() << endl;

    cout << "Minimal risk path: " << shortest_path(g, graph::vertex({0, 0}), graph::vertex({g.size_x() - 1, g.size_y() - 1})) << endl;
}

graph::graph(istream &input) {
    this->map_size_x = 0;
    this->map_size_y = 0;

    string line;
    while (getline(input, line)) {
        this->map_size_x = static_cast<int>(line.size());
        int y = this->map_size_y++;

        for (size_t i = 0; i < line.size(); ++i) {
            this->map[y][i] = static_cast<uint8_t>(line[i] - '0');
        }
    }
}

void graph::expand_map() {
    int initial_size_x = this->map_size_x;
    int initial_size_y = this->map_size_y;
    this->map_size_x *= 5;
    this->map_size_y *= 5;

    auto expand_down = [&] (int x, int y) {
        uint8_t next_value = this->map[y][x];
        for (int i = 1; i < 5; ++i) {
            if (++next_value > 9) next_value = 1;

            this->map[y + i * initial_size_y][x] = next_value;
        }
    };
    auto expand_right = [&] (int x, int y) {
        uint8_t next_value = this->map[y][x];
        for (int i = 1; i < 5; ++i) {
            if (++next_value > 9) next_value = 1;

            this->map[y][x + i * initial_size_x] = next_value;
        }
    };

    for (int y = 0; y < initial_size_y; ++y) {
        for (int x = 0; x < initial_size_x; ++x) {
            expand_down(x, y);
            expand_right(x, y);
            expand_right(x, y + initial_size_y);
            expand_right(x, y + initial_size_y * 2);
            expand_right(x, y + initial_size_y * 3);
            expand_right(x, y + initial_size_y * 4);
        }
    }
}

int graph::size_x() const {
    return this->map_size_x;
}

int graph::size_y() const {
    return this->map_size_y;
}

void graph::each_neighbour(vertex v, const function<void(vertex, int)> &block) const {
    if (v.x - 1 >= 0) {
        block({v.x - 1, v.y}, this->map[v.y][v.x - 1]);
    }
    if (v.x + 1 < this->map_size_x) {
        block({v.x + 1, v.y}, this->map[v.y][v.x + 1]);
    }
    if (v.y - 1 >= 0) {
        block({v.x, v.y - 1}, this->map[v.y - 1][v.x]);
    }
    if (v.y + 1 < this->map_size_y) {
        block({v.x, v.y + 1}, this->map[v.y + 1][v.x]);
    }
}

int shortest_path(const graph &g, graph::vertex start, graph::vertex end) {
    struct vertex_info {
        int best_path;
        bool visited;

        vertex_info(): best_path(numeric_limits<int>::max()), visited(false) {}
    };
    vector<vertex_info> vertices_info(static_cast<size_t>(g.size_x() * g.size_y()));

    auto info_for = [&] (graph::vertex v) -> vertex_info & {
        return vertices_info[static_cast<size_t>(v.x * g.size_y() + v.y)];
    };
    auto find_best_remaining_vertex = [&] () -> graph::vertex {
        int best_path_so_far = numeric_limits<int>::max();
        graph::vertex best_vertex_so_far;
        for (int x = 0; x < g.size_x(); ++x) {
            for (int y = 0; y < g.size_y(); ++y) {
                graph::vertex v = {x, y};

                if (!info_for(v).visited && info_for(v).best_path < best_path_so_far) {
                    best_path_so_far = info_for(v).best_path;
                    best_vertex_so_far = v;
                }
            }
        }

        return best_vertex_so_far;
    };

    info_for(start).best_path = 0;
    graph::vertex curr_v = start;
    while (curr_v != end) {
        info_for(curr_v).visited = true;
        g.each_neighbour(curr_v, [&](graph::vertex neighbour, int distance) {
            if (info_for(neighbour).best_path > info_for(curr_v).best_path + distance) {
                info_for(neighbour).best_path = info_for(curr_v).best_path + distance;
            }
        });

        curr_v = find_best_remaining_vertex();
    }

    return info_for(end).best_path;
}
