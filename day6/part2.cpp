#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "string_utils.h"

using namespace std;

const int n_ages = 9;
uint64_t g_population[n_ages];

void parse_input(const string &filename);
void simulate_population(int n_days);
uint64_t calculate_population();


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: <exe> <filename> <simulated days>\n";
        return 1;
    }
    const string filename = argv[1];
    const int simulated_days = atoi(argv[2]);

    parse_input(filename);

    simulate_population(simulated_days);

    cout << "Simulated population (" << simulated_days << "): " << calculate_population() << endl;
}

void parse_input(const string &filename) {
    ifstream input_file(filename);

    int age;
    while (input_file >> skip(",") >> age) {
        if (age < 0 || age >= n_ages) throw runtime_error("Invalid age");

        ++g_population[age];
    }
}

void simulate_population(int n_days) {
    for (int day = 0; day < n_days; ++day) {
        uint64_t n_spawning = g_population[0];
        for (int i_age = 1; i_age < n_ages; ++i_age) {
            g_population[i_age - 1] = g_population[i_age];
        }
        g_population[6] += n_spawning;
        g_population[8] = n_spawning;
    }
}

uint64_t calculate_population() {
    uint64_t population = 0;
    for (int i_age = 0; i_age < n_ages; ++i_age) {
        population += g_population[i_age];
    }

    return population;
}
