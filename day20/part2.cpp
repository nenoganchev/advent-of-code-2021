#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <bitset>
#include "string_utils.h"

using namespace std;

class image {
public:
    typedef bool pixel;
    typedef vector<pixel>::const_iterator iterator;

    image(const string &filename);

    int size_x() const;
    int size_y() const;
    pixel at(int x, int y) const;

    void enhance();

    iterator begin();
    iterator end();

private:
    vector<pixel> pixels;
    vector<pixel> enhancement_pixels;
    int _size_x;
    int _size_y;
    pixel oob_pixels;

    pixel compute_enhanced_pixel(int x, int y) const;
};


void print(const image &image);
ostream &operator << (ostream &os, const vector<bool> &v) { for (bool b : v) { os << b; } return os; }


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: <exe> <filename>\n";
        return 1;
    }
    const string filename = argv[1];

    image image(filename);

    cout << "Initial image:" << endl
         << endl;
    print(image);

    for (int i = 0; i < 50; ++i) {
        cout << "Enhancing (pass " << i + 1 << ")..." << endl;
        image.enhance();
    }

    cout << endl
         << "Enhanced image (final):" << endl
         << endl;
    print(image);

    int lit_pixels = 0;
    for (image::pixel px : image) {
        if (px) ++lit_pixels;
    }
    cout << endl
         << "Lit pixels: " << lit_pixels << endl;
}

image::image(const string &filename) {
    ifstream input_file(filename);

    string enhancement_line;
    getline(input_file, enhancement_line);
    for (char c : enhancement_line) {
        this->enhancement_pixels.push_back(c == '#' ? 1 : 0);
    }
    if (this->enhancement_pixels.size() != 512) throw runtime_error("Enhancement line is not 512 pixels wide");

    expect_line(input_file, "");

    this->_size_y = 0;

    string image_line;
    while (getline(input_file, image_line)) {
        this->_size_x = static_cast<int>(image_line.size());
        ++this->_size_y;

        for (char c : image_line) {
            this->pixels.push_back(c == '#' ? 1 : 0);
        }
    }

    this->oob_pixels = 0;
}

int image::size_x() const {
    return this->_size_x;
}

int image::size_y() const {
    return this->_size_y;
}

image::pixel image::at(int x, int y) const {
    if (x < 0 || y < 0 || x >= this->_size_x || y >= this->_size_y) return this->oob_pixels;

    return this->pixels[static_cast<size_t>(y * this->_size_x + x)];
}

void image::enhance() {
    const int enhanced_image_size_x = this->_size_x + 2;
    const int enhanced_image_size_y = this->_size_y + 2;
    vector<pixel> enhanced_pixels;
    enhanced_pixels.reserve(static_cast<size_t>(enhanced_image_size_x * enhanced_image_size_y));

    for (int orig_y = -1; orig_y < this->_size_y + 1; ++orig_y) {
        for (int orig_x = -1; orig_x < this->_size_x + 1; ++orig_x) {
            enhanced_pixels.push_back(this->compute_enhanced_pixel(orig_x, orig_y));
        }
    }

    this->oob_pixels = this->compute_enhanced_pixel(-2, -2);
    this->pixels = move(enhanced_pixels);
    this->_size_x = enhanced_image_size_x;
    this->_size_y = enhanced_image_size_y;
}

image::pixel image::compute_enhanced_pixel(int x, int y) const {
    bitset<9> enhancement_pixels_index;

    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            enhancement_pixels_index <<= 1;
            if (this->at(x + dx, y + dy) == 1) {
                enhancement_pixels_index[0] = 1;
            }
        }
    }

    return this->enhancement_pixels[enhancement_pixels_index.to_ulong()];
}

image::iterator image::begin() {
    return this->pixels.cbegin();
}

image::iterator image::end() {
    return this->pixels.cend();
}

void print(const image &image) {
    for (int y = 0; y < image.size_y(); ++y) {
        for (int x = 0; x < image.size_x(); ++x) {
            cout << (image.at(x, y) ? '#' : '.');
        }
        cout << endl;
    }
}
