#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

class seafloor_map {
public:
    enum tile_type {east, south, empty};

    seafloor_map(const string &filename);
    seafloor_map(size_t size_x, size_t size_y);

    bool operator == (const seafloor_map &rhs) const = default;

    size_t size_x() const;
    size_t size_y() const;
    tile_type at(size_t x, size_t y) const;
    void set(size_t x, size_t y, tile_type tile);
    void print() const;

private:
    vector<tile_type> buffer;
    size_t _size_x;
    size_t _size_y;
};

int steps_to_equilibrium(seafloor_map &map);
seafloor_map simulate_step(const seafloor_map &map);


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: <exe> <filename>\n";
        return 1;
    }
    const string filename = argv[1];

    seafloor_map map(filename);

    cout << "Initial map (x: " << map.size_x() << ", y: " << map.size_y() << "):" << endl;
    map.print();

    cout << "Equilibrium reached after: " << steps_to_equilibrium(map) << " steps" << endl;
}

seafloor_map::seafloor_map(const string &filename) {
    ifstream input_file(filename);

    auto char_to_tile = [](char c) {
        switch (c) {
        case '>': return east;
        case 'v': return south;
        case '.': return empty;
        default: throw runtime_error("Invalid tile type");
        }
    };

    this->_size_y = 0;

    string line;
    while (getline(input_file, line)) {
        this->_size_x = line.size();
        ++this->_size_y;

        for (char c : line) {
            this->buffer.push_back(char_to_tile(c));
        }
    }
}

seafloor_map::seafloor_map(size_t size_x, size_t size_y):
buffer(size_x * size_y, empty), _size_x(size_x), _size_y(size_y) {}

size_t seafloor_map::size_x() const {
    return this->_size_x;
}

size_t seafloor_map::size_y() const {
    return this->_size_y;
}

seafloor_map::tile_type seafloor_map::at(size_t x, size_t y) const {
    if (x == this->_size_x) x = 0;
    if (y == this->_size_y) y = 0;

    return this->buffer[y * this->_size_x + x];
}

void seafloor_map::set(size_t x, size_t y, tile_type tile) {
    if (x == this->_size_x) x = 0;
    if (y == this->_size_y) y = 0;

    this->buffer[y * this->_size_x + x] = tile;
}

void seafloor_map::print() const {
    auto tile_to_char = [](tile_type tile) {
        switch (tile) {
        case east: return '>';
        case south: return 'v';
        case empty: return '.';
        }
    };

    cout << endl;
    for (size_t y = 0; y < this->_size_y; ++y) {
        for (size_t x = 0; x < this->_size_x; ++x) {
            cout << tile_to_char(this->at(x, y));
        }
        cout << endl;
    }
    cout << endl;
}

int steps_to_equilibrium(seafloor_map &map) {
    int steps = 0;
    for (;;) {
        seafloor_map new_map = simulate_step(map);
        ++steps;

        if (new_map == map) return steps;

        map = move(new_map);
    }
}

seafloor_map simulate_step(const seafloor_map &map) {
    seafloor_map new_map(map.size_x(), map.size_y());

    for (size_t y = 0; y < map.size_y(); ++y) {
        for (size_t x = 0; x < map.size_x(); ++x) {
            if (map.at(x, y) == seafloor_map::east) {
                size_t new_x_pos = map.at(x + 1, y) == seafloor_map::empty ? x + 1 : x;
                new_map.set(new_x_pos, y, seafloor_map::east);
            }
        }
    }

    for (size_t y = 0; y < map.size_y(); ++y) {
        for (size_t x = 0; x < map.size_x(); ++x) {
            if (map.at(x, y) == seafloor_map::south) {
                size_t new_y_pos;
                if (map.at(x, y + 1) == seafloor_map::south || new_map.at(x, y + 1) == seafloor_map::east) {
                    new_y_pos = y;
                }
                else {
                    new_y_pos = y + 1;
                }
                new_map.set(x, new_y_pos, seafloor_map::south);
            }
        }
    }

    return new_map;
}
