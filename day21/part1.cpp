#include <iostream>
#include <fstream>
#include <tuple>
#include "string_utils.h"

using namespace std;

class dice {
public:
    dice();

    int roll();
    int rolls_count() const;

private:
    int next_roll;
    int times_rolled;
};

tuple<int, int> parse_input(const string &filename);
tuple<int, int> play_game(int p1_start_pos, int p2_start_pos, dice &dice);


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: <exe> <filename>\n";
        return 1;
    }
    const string filename = argv[1];

    auto [p1_start_pos, p2_start_pos] = parse_input(filename);

    cout << "Starting positions: " << p1_start_pos << ", " << p2_start_pos << endl;

    cout << "Starting game..." << endl;
    dice dice;
    auto [p1_score, p2_score] = play_game(p1_start_pos, p2_start_pos, dice);

    int loser_score = min(p1_score, p2_score);
    int rolls = dice.rolls_count();
    cout << "Loser score " << loser_score << " after " << rolls << " rolls (answer: " << loser_score * rolls << ")" << endl;
}

tuple<int, int> parse_input(const string &filename) {
    ifstream input_file(filename);

    int p1_start_pos, p2_start_pos;
    input_file >> expect("Player 1 starting position:") >> p1_start_pos >> skip("\n")
               >> expect("Player 2 starting position:") >> p2_start_pos;

    return {p1_start_pos, p2_start_pos};
}

tuple<int, int> play_game(int p1_start_pos, int p2_start_pos, dice &dice) {
    struct player_state {
        int pos;
        int score;
    };

    player_state states[] = {{p1_start_pos, 0}, {p2_start_pos, 0}};

    for (int i_curr_player = 0;; i_curr_player = 1 - i_curr_player) {
        int &curr_player_pos = states[i_curr_player].pos;
        int &curr_player_score = states[i_curr_player].score;

        int rolls_sum = dice.roll() + dice.roll() + dice.roll();

        curr_player_pos += rolls_sum;
        curr_player_pos %= 10;
        if (curr_player_pos == 0) curr_player_pos = 10;

        curr_player_score += curr_player_pos;

        if (curr_player_score >= 1000) break;
    }

    return {states[0].score, states[1].score};
}

dice::dice() {
    this->next_roll = 1;
    this->times_rolled = 0;
}

int dice::roll() {
    ++this->times_rolled;

    int rolled_number = this->next_roll;
    if (++this->next_roll > 100) this->next_roll = 1;

    return rolled_number;
}

int dice::rolls_count() const {
    return this->times_rolled;
}
