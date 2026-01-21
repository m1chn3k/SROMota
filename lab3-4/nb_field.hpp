#pragma once
#include <string>
#include <iostream>
#include <regex>
#include <vector>
#include <cstdint>

using namespace std;

// Normal Basis Field for m = 131
struct nb_field
{
protected:
    // 131 bits -> needs 3 chunks of 64 (3 * 64 = 192)
    uint64_t data[3] = { 0 };

    static const int mult_table[265];

public:
    nb_field() {};
    nb_field(string s);

    nb_field operator+(const nb_field& other) const;
    nb_field operator*(const nb_field& other) const;
    nb_field& operator=(const nb_field& other) = default;

    nb_field(const nb_field&) = default;
    nb_field(nb_field&&) = default;

    nb_field square() const;
    nb_field power(const nb_field& other) const;
    nb_field square_times(int i) const;

    bool check_syntax(string n);
    void print_bin() const;
    int get_highest_bit() const;
    int trace() const;

    bool mult_helper(int i, const uint64_t u[3], const uint64_t v[3]) const;
    nb_field inverse() const;
};