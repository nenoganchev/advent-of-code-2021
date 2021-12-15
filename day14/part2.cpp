#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include "string_utils.h"

using namespace std;

typedef map<string, char> insertion_rules;

struct polymer {
    map<string, uint64_t> pairs;
    map<char, uint64_t> elements;
};

tuple<polymer, insertion_rules> parse_input(const string &filename);
void grow_polymer(const insertion_rules &rules, polymer &poly);
void print_stats(const polymer &poly);


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: <exe> <filename>\n";
        return 1;
    }
    const string filename = argv[1];

    auto [polymer, rules] = parse_input(filename);

    for (int i = 0; i < 40; ++i) {
        grow_polymer(rules, polymer);
    }

    auto it_most_common = polymer.elements.cbegin();
    auto it_least_common = polymer.elements.cbegin();
    for (auto it = polymer.elements.cbegin(); it != polymer.elements.cend(); ++it) {
        if (it->second > it_most_common->second) it_most_common = it;
        if (it->second < it_least_common->second) it_least_common = it;
    }
    cout << "Most common element: " << it_most_common->first << " (" << it_most_common->second << ")" << endl
         << "Least common element: " << it_least_common->first << " (" << it_least_common->second << ")" << endl
         << "Answer: " << it_most_common->second - it_least_common->second << endl;
}

tuple<polymer, insertion_rules> parse_input(const string &filename) {
    ifstream input_file(filename);
    polymer poly;
    insertion_rules rules;

    string polymer_template;
    getline(input_file, polymer_template);

    string pair;
    for (size_t i = 0; (pair = polymer_template.substr(i, 2)).size() == 2; ++i) {
        poly.pairs[pair] += 1;
    }
    for (char c : polymer_template) {
        poly.elements[c] += 1;
    }

    expect_line(input_file, "");

    string line;
    while (getline(input_file, line)) {
        istringstream iss(line);

        string pair;
        char inserted_element;
        iss >> pair >> expect(" -> ") >> inserted_element;

        rules[pair] = inserted_element;
    }

    return {poly, rules};
}

void grow_polymer(const insertion_rules &rules, polymer &poly) {
    map<string, uint64_t> new_pairs;
    map<string, uint64_t> disappeared_pairs;
    for (const auto &[pair, count] : poly.pairs) {
        auto it_rule = rules.find(pair);
        if (it_rule != rules.end()) {
            new_pairs[string({pair[0], it_rule->second})] += count;
            new_pairs[string({it_rule->second, pair[1]})] += count;
            disappeared_pairs[pair] += count;
            poly.elements[it_rule->second] += count;
        }
    }

    for (const auto &[new_pair, count] : new_pairs) {
        poly.pairs[new_pair] += count;
    }
    for (const auto &[disappeared_pair, count] : disappeared_pairs) {
        poly.pairs[disappeared_pair] -= count;
    }
}

void print_stats(const polymer &poly) {
    cout << "Pairs:" << endl;
    for (const auto &[pair, count] : poly.pairs) {
        cout << "  " << pair << ": " << count << endl;
    }

    uint64_t total = 0;
    cout << endl
         << "Elements:" << endl;
    for (const auto &[element, count] : poly.elements) {
        cout << "  " << element << ": " << count << endl;
        total += count;
    }
    cout << "  Total: " << total << endl;
}
