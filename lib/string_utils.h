#include <vector>
#include <string>
#include <istream>

using namespace std;

vector<string> split(const string &string_to_split, const string &separator);

struct expect {
    expect(const string &expected_string);
    friend istream & operator >> (istream &is, const expect &manip);

    string expected_str;
};
