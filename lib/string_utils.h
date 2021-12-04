#include <vector>
#include <string>
#include <istream>

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
vector<string> split(const string &string_to_split, const string &separator);
