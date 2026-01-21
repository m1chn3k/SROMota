#include "number.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>

using namespace std;
using Clock = std::chrono::steady_clock;
using std::chrono::microseconds;
using std::chrono::duration_cast;

int main()
{
	string s_a = "A1B2C3D4E5F6";
	string s_b = "123456789ABC";
	string s_n = "FFFFFFFFFFFFFFFF"; 

	cout << "--- Initializing Numbers ---" << endl;
	number a(s_a);
	cout << "A: "; a.print_hex();

	number b(s_b);
	cout << "B: "; b.print_hex();

	number n(s_n);
	cout << "N (Modulus): "; n.print_hex();
	cout << "----------------------------" << endl;

	cout << "[1] Testing GCD (A, B):" << endl;
	number c = a.gcd(b);
	c.print_hex();

	cout << "[2] Testing AddMod (A + B) % N:" << endl;
	number d = n.add_mod(a, b);
	d.print_hex();

	cout << "[3] Testing SubMod (A - B) % N:" << endl;
	number e = n.sub_mod(a, b);
	e.print_hex();

	cout << "[4] Testing MulMod (A * B) % N:" << endl;
	number f = n.mul_mod(a, b);
	f.print_hex();

	cout << "[5] Testing PowMod (A ^ B) % N:" << endl;

	auto start = Clock::now();

	number g = n.pow_mod(a, b);

	auto end = Clock::now();
	auto diff = duration_cast<microseconds>(end - start);

	g.print_hex();
	cout << "Time taken: " << diff.count() << " mks" << endl;

	cout << "[6] Testing Division (A / B):" << endl;
	number div_res = a / b;
	div_res.print_hex();

	return 0;
}