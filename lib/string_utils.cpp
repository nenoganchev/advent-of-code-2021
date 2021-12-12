#include "string_utils.h"
#include <cstring>

//
// skip implementation
//

skip::skip(const char *s): skipped_characters(s) {}

istream & operator >> (istream &is, const skip &manip) {
    while (is && strchr(manip.skipped_characters, is.peek())) is.ignore();
    return is;
}

//
// expect
//

expect::expect(const string &str): expected_str(str) {}

istream & operator >> (istream &is, const expect &manip) {
    for (char expected_char : manip.expected_str) {
        char read_char;
        if (!(is.get(read_char) && read_char == expected_char)) {
            throw runtime_error("expect failed");
        }
    }

    return is;
}

//
// functions implementation
//

void expect_line(istream &is, const string &expected_line) {
    string line;
    getline(is, line);
    if (line != expected_line) throw runtime_error("expect_line failed");
}

vector<string> split(const string &string_to_split, const typename::string &separator) {
    vector<typename::string> result;

    size_t i_begin = 0;
    size_t i_sep;
    do
    {
        i_sep = string_to_split.find(separator, i_begin);
        result.push_back(string_to_split.substr(i_begin, i_sep - i_begin));
        i_begin = i_sep + separator.size();
    }
    while (i_sep != string::npos);

    return result;
}

string join(const vector<string> &parts, const string &delimiter) {
    string joined_string;
    string curr_delimiter;

    for (const string &part : parts) {
        joined_string += curr_delimiter + part;
        curr_delimiter = delimiter;
    }

    return joined_string;
}
