#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <regex>
#include <algorithm>
#include <cstdint>

using namespace std;

struct field
{
protected:
    // 283 біти -> це 5 блоків по 64 (5 * 64 = 320)
    static constexpr int num_words = 5;
    uint64_t data[num_words] = { 0 };

public:
    field() {};
    field(string s);

    field operator+(const field& other) const;
    field operator*(const field& other) const;
    field& operator=(const field& other) = default; // default is fine

    field(const field&) = default;
    field(field&&) = default;

    void shift_once();
    bool check_syntax(string n);
    int get_highest_bit() const;
    void print_bin() const;

    field power(const field& other) const;
    field square() const;
    int trace() const;
    field inverse() const;
};