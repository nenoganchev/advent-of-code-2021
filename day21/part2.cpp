#include <iostream>
#include <fstream>
#include <map>
#include <tuple>
#include "string_utils.h"

using namespace std;

struct player_state {
    int pos;
    int score;
};

struct game_state {
    player_state players[2];
    int i_next_player;

    bool operator < (const game_state &rhs) const;
};

tuple<int, int> parse_input(const string &filename);
tuple<uint64_t, uint64_t> play_game_with_recursion(int p1_start_pos, int p2_start_pos);
tuple<uint64_t, uint64_t> calculate_wins_for(const game_state &state);
tuple<uint64_t, uint64_t> play_game_without_recursion(int p1_start_pos, int p2_start_pos);


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: <exe> <filename>\n";
        return 1;
    }
    const string filename = argv[1];

    auto [p1_start_pos, p2_start_pos] = parse_input(filename);

    cout << "Starting positions: " << p1_start_pos << ", " << p2_start_pos << endl;

    cout << "Playing recursive game..." << endl;

    auto [p1_wins, p2_wins] = play_game_with_recursion(p1_start_pos, p2_start_pos);
    cout << "P1 wins: " << p1_wins << endl
         << "P2 wins: " << p2_wins << endl;

    cout << endl
         << "Playing iterative game..." << endl;

    tie(p1_wins, p2_wins) = play_game_without_recursion(p1_start_pos, p2_start_pos);
    cout << "P1 wins: " << p1_wins << endl
         << "P2 wins: " << p2_wins << endl;
}

tuple<int, int> parse_input(const string &filename) {
    ifstream input_file(filename);

    int p1_start_pos, p2_start_pos;
    input_file >> expect("Player 1 starting position:") >> p1_start_pos >> skip("\n")
               >> expect("Player 2 starting position:") >> p2_start_pos;

    return {p1_start_pos, p2_start_pos};
}

tuple<uint64_t, uint64_t> play_game_with_recursion(int p1_start_pos, int p2_start_pos) {
    game_state initial_state = {
        .players = {{.pos = p1_start_pos, .score = 0}, {.pos = p2_start_pos, .score = 0}},
        .i_next_player = 0
    };

    return calculate_wins_for(initial_state);
}

tuple<uint64_t, uint64_t> calculate_wins_for(const game_state &state) {
    // maps (rolls sum) -> (numbers of universes) after each turn of 3 rolls
    static const map<int, unsigned> rolls_sum_distributions = {
        {3, 1},
        {4, 3},
        {5, 6},
        {6, 7},
        {7, 6},
        {8, 3},
        {9, 1}
    };
    static map<game_state, tuple<uint64_t, uint64_t>> cache;

    if (state.players[0].score >= 21) return {1, 0};
    if (state.players[1].score >= 21) return {0, 1};
    if (cache.contains(state)) return cache[state];

    uint64_t p1_wins = 0;
    uint64_t p2_wins = 0;
    for (auto [rolls_sum, split_factor] : rolls_sum_distributions) {
        game_state new_state = state;
        int &turn_player_pos = new_state.players[new_state.i_next_player].pos;
        int &turn_player_score = new_state.players[new_state.i_next_player].score;
        new_state.i_next_player = 1 - new_state.i_next_player;

        turn_player_pos += rolls_sum;
        if (turn_player_pos > 10) turn_player_pos -= 10;
        turn_player_score += turn_player_pos;

        auto [new_state_p1_wins, new_state_p2_wins] = calculate_wins_for(new_state);
        p1_wins += new_state_p1_wins * split_factor;
        p2_wins += new_state_p2_wins * split_factor;
    }

    cache[state] = {p1_wins, p2_wins};
    return {p1_wins, p2_wins};
}

tuple<uint64_t, uint64_t> play_game_without_recursion(int p1_start_pos, int p2_start_pos) {
    vector<tuple<game_state, uint64_t>> in_progress;
    uint64_t wins[2] = {0, 0};

    game_state initial_state = {
        .players = {{.pos = p1_start_pos, .score = 0}, {.pos = p2_start_pos, .score = 0}},
        .i_next_player = 0
    };
    in_progress.push_back({initial_state, 1});

    // maps (rolls sum) -> (numbers of universes) after each turn of 3 rolls
    static const map<int, unsigned> rolls_sum_distributions = {
        {3, 1},
        {4, 3},
        {5, 6},
        {6, 7},
        {7, 6},
        {8, 3},
        {9, 1}
    };

    while (!in_progress.empty()) {
        auto [curr_state, universes] = in_progress.back();
        in_progress.pop_back();

        for (auto [rolls_sum, split_factor] : rolls_sum_distributions) {
            game_state new_state = curr_state;
            int &turn_player_pos = new_state.players[new_state.i_next_player].pos;
            int &turn_player_score = new_state.players[new_state.i_next_player].score;
            uint64_t &turn_player_wins = wins[new_state.i_next_player];
            new_state.i_next_player = 1 - new_state.i_next_player;

            turn_player_pos += rolls_sum;
            if (turn_player_pos > 10) turn_player_pos -= 10;
            turn_player_score += turn_player_pos;

            if (turn_player_score >= 21) {
                turn_player_wins += universes * split_factor;
            }
            else {
                in_progress.push_back({new_state, universes * split_factor});
            }
        }
    }

    return {wins[0], wins[1]};
}

bool game_state::operator < (const game_state &rhs) const {
    return make_tuple(this->players[0].pos, this->players[0].score, this->players[1].pos, this->players[1].score, this->i_next_player) <
           make_tuple(rhs.players[0].pos, rhs.players[0].score, rhs.players[1].pos, rhs.players[1].score, rhs.i_next_player);
}
