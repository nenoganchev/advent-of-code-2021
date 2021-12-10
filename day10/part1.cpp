#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "string_utils.h"

using namespace std;

struct parse_line_result {
    enum {ok, incomplete, corrupted} status;
    char error_char;
    const char *pc_next;
};

vector<string> parse_input(const string &filename);
int calculate_syntax_error_score(const vector<string> &lines);
parse_line_result parse_line(const string &line);
void consume_chunk(const char *pc_chunk_start, parse_line_result &result);
bool chunk_open_char(char c);
bool chunk_open_close_chars_match(char open, char close);


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: <exe> <filename>\n";
        return 1;
    }
    const string filename = argv[1];

    vector<string> lines = parse_input(filename);

    cout << "Syntax error score: " << calculate_syntax_error_score(lines) << endl;
}

vector<string> parse_input(const string &filename) {
    ifstream input_file(filename);
    vector<string> lines;

    string line;
    while (getline(input_file, line)) {
        lines.push_back(line);
    }

    return lines;
}

int calculate_syntax_error_score(const vector<string> &lines) {
    int error_score = 0;

    auto score_for = [] (char error_char) {
        switch (error_char) {
            case ')': return 3;
            case ']': return 57;
            case '}': return 1197;
            case '>': return 25137;
            default: throw runtime_error("Invalid error char");
        }
    };

    for (const string &line : lines) {
        parse_line_result result = parse_line(line);
        if (result.status == parse_line_result::corrupted) {
            error_score += score_for(result.error_char);
        }
    }

    return error_score;
}

parse_line_result parse_line(const string &line) {
    parse_line_result result = {.status = parse_line_result::ok};

    for (const char *pc_next_chunk_start = line.data();
         result.status == parse_line_result::ok && *pc_next_chunk_start != '\0';
         pc_next_chunk_start = result.pc_next) {

        consume_chunk(pc_next_chunk_start, result);
    }

    return result;
}

void consume_chunk(const char *pc_chunk_start, parse_line_result &result) {
    if (!chunk_open_char(*pc_chunk_start)) {
        result.status = parse_line_result::corrupted;
        result.error_char = *pc_chunk_start;
        return;
    }

    const char *pc_next = pc_chunk_start + 1;
    while (chunk_open_char(*pc_next)) {
        consume_chunk(pc_next, result);
        if (result.status != parse_line_result::ok) return;

        pc_next = result.pc_next;
    }
    const char *pc_chunk_end = pc_next;

    if (!chunk_open_close_chars_match(*pc_chunk_start, *pc_chunk_end)) {
        result.status = *pc_chunk_end == '\0' ? parse_line_result::incomplete : parse_line_result::corrupted;
        result.error_char = *pc_chunk_end;
        return;
    }

    result.pc_next = pc_chunk_end + 1;
}

bool chunk_open_char(char c) {
    switch (c) {
        case '(':
        case '[':
        case '{':
        case '<':
            return true;
    }

    return false;
}

bool chunk_open_close_chars_match(char open, char close) {
    return (open == '(' && close == ')') || (open == '[' && close == ']') ||
           (open == '{' && close == '}') || (open == '<' && close == '>');
}
