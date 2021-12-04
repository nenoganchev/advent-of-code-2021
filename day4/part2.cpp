#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <tuple>
#include <algorithm>
#include "string_utils.h"

using namespace std;

const int board_size = 5;

struct board {
    uint32_t numbers[board_size][board_size];
    bool won;
};

tuple<vector<uint32_t>, vector<board>> parse_input(const string &filename);
vector<uint32_t> parse_drawn_numbers(ifstream &input_file);
vector<board> parse_boards(ifstream &input_file);
tuple<const board &, uint32_t> find_last_winning_board(vector<board> &boards, const vector<uint32_t> &drawn_numbers);
uint32_t calculate_board_score(const board &board, uint32_t last_drawn_number);
tuple<int, int> mark_number_in_board(board &board, uint32_t number);
bool board_wins(const board &board, int i_checked_row, int i_checked_col);
void mark(uint32_t &number);
bool is_marked(uint32_t number);


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: <exe> <filename>\n";
        return 1;
    }
    const string filename = argv[1];

    auto [drawn_numbers, boards] = parse_input(filename);

    auto [last_winning_board, last_drawn_number] = find_last_winning_board(boards, drawn_numbers);
    cout << "Last winning board score: " << calculate_board_score(last_winning_board, last_drawn_number) << endl;
}

tuple<vector<uint32_t>, vector<board>> parse_input(const string &filename) {
    ifstream input_file(filename);

    vector<uint32_t> drawn_numbers = parse_drawn_numbers(input_file);
    expect_line(input_file, "");
    vector<board> boards = parse_boards(input_file);

    return {drawn_numbers, boards};
}

vector<uint32_t> parse_drawn_numbers(ifstream &input_file) {
    vector<uint32_t> drawn_numbers;

    string line;
    getline(input_file, line);

    istringstream iss(line);
    uint32_t number;
    while (iss >> skip(",") >> number) {
        drawn_numbers.push_back(number);
    }

    return drawn_numbers;
}

vector<board> parse_boards(ifstream &input_file) {
    string line;
    vector<board> boards;

    while (input_file) {
        board board;
        board.won = false;

        for (int i_row = 0; i_row < board_size; ++i_row) {
            for (int i_col = 0; i_col < board_size; ++i_col) {
                input_file >> board.numbers[i_row][i_col];
            }
        }
        getline(input_file, line);

        boards.push_back(board);
    }

    return boards;
}

tuple<const board &, uint32_t> find_last_winning_board(vector<board> &boards, const vector<uint32_t> &drawn_numbers) {
    for (uint32_t drawn_number : drawn_numbers) {
        for (board &curr_board : boards) {
            auto [i_found_row, i_found_col] = mark_number_in_board(curr_board, drawn_number);
            if (board_wins(curr_board, i_found_row, i_found_col)) {
                curr_board.won = true;
                if (all_of(boards.cbegin(), boards.cend(), [](const board &b) { return b.won; })) {
                    return {curr_board, drawn_number};
                }
            }
        }
    }

    throw runtime_error("No winning boards found");
}

uint32_t calculate_board_score(const board &board, uint32_t last_drawn_number) {
    uint32_t unmarked_sum = 0;
    for (int i = 0; i < board_size; ++i) {
        for (int j = 0; j < board_size; ++j) {
            if (!is_marked(board.numbers[i][j])) {
                unmarked_sum += board.numbers[i][j];
            }
        }
    }

    return unmarked_sum * last_drawn_number;
}

tuple<int, int> mark_number_in_board(board &board, uint32_t number) {
    for (int i_row = 0; i_row < board_size; ++i_row) {
        for (int i_col = 0; i_col < board_size; ++i_col) {
            if (board.numbers[i_row][i_col] == number) {
                mark(board.numbers[i_row][i_col]);
                return {i_row, i_col};
            }
        }
    }

    return {-1, -1};
}

bool board_wins(const board &board, int i_checked_row, int i_checked_col) {
    if (i_checked_row == -1) return false;

    bool whole_row_marked = true;
    for (int i = 0; i < board_size; ++i) {
        if (!is_marked(board.numbers[i_checked_row][i])) {
            whole_row_marked = false;
            break;
        }
    }
    if (whole_row_marked) return true;

    bool whole_col_marked = true;
    for (int i = 0; i < board_size; ++i) {
        if (!is_marked(board.numbers[i][i_checked_col])) {
            whole_col_marked = false;
            break;
        }
    }
    if (whole_col_marked) return true;

    return false;
}

void mark(uint32_t &number) {
    number |= 0x80000000;
}

bool is_marked(uint32_t number) {
    return number & 0x80000000;
}
