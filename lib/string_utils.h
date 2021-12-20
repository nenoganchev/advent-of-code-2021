#include <vector>
#include <string>
#include <istream>
#include <sstream>

using namespace std;

struct skip {
    skip(const char *skipped_characters);
    friend istream & operator >> (istream &is, const skip &manip);

    const char *skipped_characters;
};

struct expect {
    expect(const string &expected_string);
    friend istream & operator >> (istream &is, const expect &manip);

    string expected_str;
};

void expect_line(istream &is, const string &expected_line);
void expect_line_starting_with(istream &is, const string &expected_line_start);
vector<string> split(const string &string_to_split, const string &separator);

template<class T>
string join(const vector<T> &parts, const string &delimiter) {
    ostringstream join_stream;
    string curr_delimiter;

    for (const T &part : parts) {
        join_stream << curr_delimiter << part;
        curr_delimiter = delimiter;
    }

    return join_stream.str();
}
