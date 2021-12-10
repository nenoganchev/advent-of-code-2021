#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "string_utils.h"

using namespace std;

struct parse_line_result {
    enum {ok, incomplete, corrupted} status;
    string completion_string;
    char error_char;
    const char *pc_next;
};

vector<string> parse_input(const string &filename);
uint64_t calculate_autocomplete_score(const vector<string> &lines);
parse_line_result parse_line(const string &line);
void consume_chunk(const char *pc_chunk_start, parse_line_result &result);
bool chunk_open_char(char c);
char close_char_for(char c);
bool chunk_open_close_chars_match(char open, char close);


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: <exe> <filename>\n";
        return 1;
    }
    const string filename = argv[1];

    vector<string> lines = parse_input(filename);

    cout << "Syntax error score: " << calculate_autocomplete_score(lines) << endl;
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

uint64_t calculate_autocomplete_score(const vector<string> &lines) {
    vector<uint64_t> scores;

    auto score_for = [] (const string &completion_string) {
        uint64_t score = 0;
        for (char c : completion_string) {
            score *= 5;
            switch (c) {
                case ')': score += 1; break;
                case ']': score += 2; break;
                case '}': score += 3; break;
                case '>': score += 4; break;
                default: throw runtime_error("Invalid completion char");
            }
        }

        return score;
    };

    for (const string &line : lines) {
        parse_line_result result = parse_line(line);
        if (result.status == parse_line_result::incomplete) {
            scores.push_back(score_for(result.completion_string));
        }
    }

    sort(scores.begin(), scores.end());
    return scores[scores.size() / 2];
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
        if (result.status == parse_line_result::corrupted) return;

        pc_next = result.pc_next;
    }
    const char *pc_chunk_end = pc_next;

    if (chunk_open_close_chars_match(*pc_chunk_start, *pc_chunk_end)) {
        result.pc_next = pc_chunk_end + 1;
    }
    else if (*pc_chunk_end == '\0') {
        result.status = parse_line_result::incomplete;
        result.completion_string += close_char_for(*pc_chunk_start);
        result.pc_next = pc_chunk_end;
    }
    else {
        result.status = parse_line_result::corrupted;
        result.error_char = *pc_chunk_end;
    }
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

char close_char_for(char c) {
    switch (c) {
        case '(': return ')';
        case '[': return ']';
        case '{': return '}';
        case '<': return '>';
        default: throw runtime_error("Invalid open char");
    }
}

bool chunk_open_close_chars_match(char open, char close) {
    return (open == '(' && close == ')') || (open == '[' && close == ']') ||
           (open == '{' && close == '}') || (open == '<' && close == '>');
}
