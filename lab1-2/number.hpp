#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <random>
#include <regex>
#include <cstdint>
#include <chrono>

using namespace std;
using namespace std::chrono;

struct double_number;

struct number
{
	friend struct double_number;

protected:
	static constexpr int len = 64;   
	static constexpr int width = 32; 

	uint64_t _carry = 0;
	uint32_t words[len] = { 0 };

public:
	number() = default;
	number(string s);
	number(const number&) = default;
	number(number&&) = default;
	number& operator=(const number&) = default;

	static bool check_syntax(string n);
	void print_raw() const;
	void print_hex() const;
	string generate_hex(const int len);

	int high_word() const;
	int high_bit() const;
	int* to_bits() const;

	int compare(number other) const;
	bool operator==(const number& other) const;

	void shift_words_left(int k);
	void shift_words_right(int k);
	void shift_bits_left(int k);
	void shift_right_1();
	void shift_left_1(); // used in GCD

	number operator+ (const number& other);
	number operator+ (const uint32_t& k);
	number operator- (const number& other);
	number operator* (const number& other);
	number operator/ (const number& other);

	number operator<< (int k);
	number operator>> (int k);

	number mul_word(uint32_t k) const;
	number square(); 
	number gcd(number other);
	number power(const number& other);

	number add_mod(number a, number b);
	number sub_mod(number a, number b);
	number mul_mod(number a, number b);

	number barrett_reduction(double_number x, number n, number mu);
	number pow_mod(number base, number exponent); 

	double_number double_mult(const number& other) const;
	double_number to_double_number() const;
	double_number lcm(number other); 
};

struct double_number
{
	friend struct number;

protected:
	static constexpr int _size = 129; 
	static constexpr int _width = 32; 

	uint64_t _carry = 0;


	uint32_t _data[_size] = { 0 };

public:

	double_number() = default;
	double_number(string s);
	double_number(const double_number&) = default;
	double_number(double_number&&) = default;
	double_number& operator=(const double_number&) = default;

	static bool check_syntax(string n);
	void print_raw() const;
	void print_hex() const;

	int get_highest_word() const;
	int get_highest_bit() const;

	int compare(double_number other) const;
	bool operator==(const double_number& other) const;

	void shift_words_left(int k);
	void shift_words_right(int k);
	void shift_bits_left(int k);

	double_number operator+ (const double_number& other);
	double_number operator+ (const uint32_t& k);
	double_number operator- (const double_number& other);
	double_number operator* (const double_number& other);
	double_number operator/ (const double_number& other);

	double_number operator<< (int k);
	double_number operator>> (int k);
	double_number mul_scalar(uint32_t k) const;


	number to_number() const;
};