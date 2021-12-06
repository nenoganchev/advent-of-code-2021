#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "string_utils.h"

using namespace std;

const int n_ages = 9;
int g_population[n_ages];

void parse_input(const string &filename);
void simulate_population(int n_days);
int calculate_population();


int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: <exe> <filename>\n";
        return 1;
    }
    const string filename = argv[1];

    parse_input(filename);

    const int simulated_days = 80;
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
        int n_spawning = g_population[0];
        for (int i_age = 1; i_age < n_ages; ++i_age) {
            g_population[i_age - 1] = g_population[i_age];
        }
        g_population[6] += n_spawning;
        g_population[8] = n_spawning;
    }
}

int calculate_population() {
    int population = 0;
    for (int i_age = 0; i_age < n_ages; ++i_age) {
        population += g_population[i_age];
    }

    return population;
}
