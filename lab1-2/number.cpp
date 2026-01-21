#include "number.hpp"

// regex check bc users can't be trusted
bool number::check_syntax(string n)
{
	regex hex_pattern("^[A-F0-9]+$");
	return regex_match(n, hex_pattern);
}

number::number(string s)
{
	int str_len = (int)s.size(); // Fixed warning C4267
	if (str_len > 512)
	{
		cout << "Daddy it's too big for us" << endl;
		return;
	}

	if (!check_syntax(s))
	{
		cout << "Check your words! Invalid hex." << endl;
		return;
	}

	// parsing the hex string backwards
	for (int i = 0; i < len; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			int idx = str_len - (i * 8 + j) - 1;
			if (idx < 0)
			{
				break;
			}
			// ascii magic: convert char to int
			else if (int(s[idx]) < 58)
			{
				words[i] += (s[idx] - 48) << (j * 4);
			}
			else
			{
				words[i] += (s[idx] - 55) << (j * 4);
			}
		}
	}
	s.clear();
}

int number::high_word() const
{
	for (int i = len - 1; i > -1; i--)
	{
		if (words[i] != 0)
		{
			return i;
		}
	}
	return -1;
}

int number::high_bit() const
{
	for (int i = len - 1; i > -1; i--)
	{
		if (words[i] != 0)
		{
			for (int j = 31; j > -1; j--)
			{
				if (words[i] & (1u << j))
				{
					return i * 32 + j;
				}
			}
		}
	}
	return -1;
}

void number::print_raw() const
{
	for (int i = 0; i < len; i++)
	{
		cout << words[i] << " ";
	}
	cout << endl;
}

void number::print_hex() const
{
	cout << endl;
	char hex_map[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	int k = high_word();

	if (k >= 0)
	{
		for (int i = k; i > -1; i--)
		{
			uint32_t temp = words[i];
			for (int j = 7; j > -1; j--)
			{
				uint32_t t = temp >> (4 * j);
				cout << hex_map[t];
				temp = temp & ~(t << (4 * j));
			}
		}
	}
	cout << endl;
}

int number::compare(number other) const
{
	int i = len - 1;
	while (words[i] == other.words[i])
	{
		i--;
		if (i == -1)
		{
			return 0; // they are equal
		}
	}

	if (words[i] > other.words[i])
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

void number::shift_words_left(int k)
{
	if (len < k)
	{
		// wipe everything if shift is huge
		for (int i = 0; i < len; i++)
		{
			words[i] = 0;
		}
		return;
	}
	if (k < 0)
	{
		cout << "negative shift? bruh";
		return;
	}
	for (int i = len - 1; i >= k; i--)
	{
		words[i] = words[i - k];
	}
	for (int i = k - 1; i >= 0; i--)
	{
		words[i] = 0;
	}
}

void number::shift_words_right(int k)
{
	if (len <= k)
	{
		for (int i = 0; i < len; i++)
		{
			words[i] = 0;
		}
		return;
	}

	for (int i = 0; i < len - k; i++)
	{
		words[i] = words[i + k];
	}

	for (int i = len - k; i < len; i++)
	{
		words[i] = 0;
	}
}

void number::shift_bits_left(int k)
{
	int top_word = this->high_word();
	number temp_res;

	if (len * width < k)
	{
		for (int i = 0; i < len; i++) words[i] = 0;
		return;
	}
	if (k < 0)
	{
		cout << "error: negative shift";
		return;
	}

	int w_shift = k / width;
	int b_shift = k % width;

	if (b_shift == 0)
	{
		this->shift_words_left(k / width);
		return;
	}
	else
	{
		for (int i = 0; i <= top_word; i++)
		{
			if (i + w_shift < len)
			{
				temp_res.words[i + w_shift] |= (words[i] << b_shift);
			}
			// handle bit overflow to next word
			if (i + w_shift + 1 < len)
			{
				temp_res.words[i + w_shift + 1] |= (words[i] >> (width - b_shift));
			}
		}
	}
	*this = temp_res;
}


bool number:: operator==(const number& other) const
{
	if (this->compare(other) == 0)
	{
		return true;
	}
	return false;
}

number number::operator+(const number& other)
{
	number res("0");
	uint64_t carry = 0;

	for (int i = 0; i < len; i++)
	{
		uint64_t temp = uint64_t(this->words[i]) + uint64_t(other.words[i]) + carry;
		res.words[i] = (uint32_t)(temp & ((uint32_t)-1)); // Cast added
		carry = temp >> width;
	}
	res._carry = carry;
	return res;
}

number number::operator+(const uint32_t& k)
{
	number res;
	uint64_t carry = 0;

	uint64_t temp = uint64_t(this->words[0]) + uint64_t(k);
	res.words[0] = (uint32_t)(temp & ((uint32_t)-1));
	carry = temp >> width;

	for (int i = 1; i < len; i++)
	{
		if (carry == 0)
		{
			for (int j = i; j < number::len; j++)
			{
				res.words[j] = this->words[j];
			}
			break;
		}
		temp = uint64_t(this->words[i]) + carry;
		res.words[i] = (uint32_t)(temp & ((uint32_t)-1));
		carry = temp >> width;
	}
	res._carry = carry;
	return res;
}

number number::operator-(const number& other)
{
	number res;

	if (this->compare(other) == -1)
	{
		number max_val("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
		int64_t borrow = 0;
		for (int i = 0; i < len; i++)
		{
			int64_t temp = int64_t(other.words[i]) - int64_t(words[i]) - borrow;

			if (temp >= 0)
			{
				res.words[i] = (uint32_t)temp; // Cast
				borrow = 0;
			}
			else
			{
				res.words[i] = (uint32_t)(temp + (uint64_t(1) << width)); // Cast
				borrow = 1;
			}
		}
		res = max_val - res;
		res = res + 1;
		return res;
	}

	int64_t borrow = 0;
	for (int i = 0; i < len; i++)
	{
		int64_t temp = int64_t(words[i]) - int64_t(other.words[i]) - borrow;

		if (temp >= 0)
		{
			res.words[i] = (uint32_t)temp; // Cast
			borrow = 0;
		}
		else
		{
			res.words[i] = (uint32_t)(temp + (uint64_t(1) << width)); // Cast
			borrow = 1;
		}
	}
	return res;
}

number number::operator* (const number& other)
{
	number res;
	for (int i = 0; i < len; i++)
	{
		number temp = this->mul_word(other.words[i]);
		temp.shift_words_left(i);
		res = res + temp;
	}
	return res;
}

number number::mul_word(uint32_t k) const
{
	number res;
	uint32_t carry = 0;
	for (int i = 0; i < len; i++)
	{
		uint64_t temp = (uint64_t(words[i]) * k) + carry;
		res.words[i] = (uint32_t)(temp & ((uint32_t)-1));
		carry = (uint32_t)(temp >> width);
	}
	res._carry = carry;
	return res;
}

number number::operator/ (const number& other)
{
	number res;
	int k = other.high_bit();
	number rem = *this;
	number quot;

	while (rem.compare(other) != -1)
	{
		int t = rem.high_bit();
		res = other;
		res.shift_bits_left(t - k);

		if (rem.compare(res) == -1)
		{
			t--;
			res = other;
			res.shift_bits_left(t - k);
		}
		number temp_one("1");
		temp_one.shift_bits_left(t - k);
		rem = rem - res;
		quot = quot + temp_one;
	}
	return quot;
}

number number::square()
{
	number res;

	for (int i = 0; i < len; i++)
	{
		uint64_t carry = 0;
		for (int j = i + 1; j < len; j++)
		{
			uint64_t temp = ((uint64_t)this->words[j] * (uint64_t)this->words[i]);
			uint64_t temp_2 = temp << 1;
			if (i + j < 64)
			{
				res.words[i + j] += (uint32_t)((temp_2 & ((uint32_t)-1)) + carry); // Cast
				carry = uint64_t(uint64_t(res.words[i + j]) + temp_2) >> width;
			}
			if ((temp & (uint64_t(1) << 63)) != 0)
			{
				carry |= (uint64_t(1) << width);
			}
		}
	}

	for (int i = 0; i < len; i++)
	{
		uint64_t temp = (uint64_t)this->words[i] * (uint64_t)this->words[i];

		if (2 * i < 64)
		{
			res.words[2 * i] += (uint32_t)temp;
		}
		if (2 * i < 63)
		{
			res.words[2 * i + 1] += (uint32_t)(temp >> width);
		}
	}

	return res;
}

number number::power(const number& other)
{
	number res("1");
	int p[512] = { 0 };
	int k = high_word();

	for (int i = k; i > -1; i--)
	{
		uint32_t temp = other.words[i];
		for (int j = 7; j > -1; j--)
		{
			uint32_t t = temp >> (4 * j);
			p[i * 8 + j] = t;
			temp = temp & ~(t << (4 * j));
		}
	}

	number table[16];
	table[0] = res;
	table[1] = *this;
	for (int i = 2; i < 16; i++)
	{
		table[i] = table[i - 1] * (*this);
	}

	for (int i = 511; i > 0; i--)
	{
		res = res * table[p[i]];
		for (int z = 0; z < 4; z++)
		{
			res = res * res;
		}
	}
	res = res * table[p[0]];

	return res;
}

number number::operator<<(int k)
{
	int a = this->high_word();
	number temp_res;

	if (len * width < k)
	{
		return temp_res;
	}

	if (k < 0)
	{
		cout << "Error: impossible to shift negative." << endl;
		return *this;
	}

	int w_shift = k / width;
	int b_shift = k % width;

	if (b_shift == 0)
	{
		this->shift_words_left(w_shift);
		return *this;
	}
	else
	{
		for (int i = 0; i <= a; i++)
		{
			if (i + w_shift < len)
			{
				temp_res.words[i + w_shift] |= (words[i] << b_shift);
			}

			if (i + w_shift + 1 < len)
			{
				temp_res.words[i + w_shift + 1] |= (words[i] >> (width - b_shift));
			}
		}
	}

	*this = temp_res;
	return *this;
}

number number::operator>>(int k)
{
	int a = this->high_word();
	if (a == -1)
	{
		return *this;
	}
	number temp_res;

	if (k < 0)
	{
		cout << "Error: Negative shift amount." << endl;
		return *this;
	}

	if (len * width <= k)
	{
		return temp_res;
	}

	int w_shift = k / width;
	int b_shift = k % width;

	if (b_shift == 0)
	{
		this->shift_words_right(w_shift);
		return *this;
	}
	else
	{
		for (int i = a; i >= 0; i--)
		{
			if (i - w_shift >= 0)
			{
				temp_res.words[i - w_shift] |= (words[i] >> b_shift);
			}
			if (i - w_shift - 1 >= 0)
			{
				temp_res.words[i - w_shift - 1] |= (words[i] << (width - b_shift));
			}
		}
	}
	*this = temp_res;
	return *this;
}

int* number::to_bits() const
{
	static int p[512] = { 0 };
	int k = high_word();
	for (int i = k; i > -1; i--)
	{
		cout << " ";
		uint32_t temp = words[i];
		for (int j = 7; j > -1; j--)
		{
			uint32_t t = temp >> (4 * j);
			p[k - i] = t;
			temp = temp & ~(t << (4 * j));
		}
	}
	return p;
}

string number::generate_hex(const int len)
{
	static const char hex_chars[] = "0123456789ABCDEF";

	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> distrib(0, sizeof(hex_chars) - 2);

	string result;
	result.reserve(len);
	for (int i = 0; i < len; ++i)
	{
		result += hex_chars[distrib(gen)];
	}

	return result;
}

void number::shift_right_1()
{
	for (int i = 0; i < len - 1; i++)
	{
		words[i] = (words[i] >> 1) | (words[i + 1] << (width - 1));
	}
	words[len - 1] = words[len - 1] >> 1;
}

void number::shift_left_1()
{
	for (int i = len - 1; i > 0; i--)
	{
		words[i] = (words[i] << 1) | (words[i - 1] >> (width - 1));
	}
	words[0] = words[0] << 1;
}

// Modular Arithmetic

bool is_even(uint32_t a)
{
	return (a & 1) == 0;
}

number number::add_mod(number a, number b)
{
	number c = a + b;

	// if result >= modulus (this), reduce it
	if (c.compare(*this) != -1)
	{
		c = c - (*this * (c / *this));
	}

	return c;
}

number number::sub_mod(number a, number b)
{
	number c = a - b;

	if (c.compare(*this) != -1)
	{
		c = c - (*this * (c / *this));
	}

	return c;
}

number number::mul_mod(number a, number b)
{
	double_number p = a.double_mult(b);
	double_number n = this->to_double_number();

	if (p.compare(n) != -1)
	{
		p = p - (n * (p / n));
	}

	return p.to_number();
}

number number::gcd(number b)
{
	number a = *this;
	number d("1");

	while (is_even(a.words[0]) && is_even(b.words[0]))
	{
		a.shift_right_1();
		b.shift_right_1();
		d.shift_left_1();
	}

	while (is_even(a.words[0]))
	{
		a.shift_right_1();
	}

	while (b.high_word() != -1)
	{
		while (is_even(b.words[0]))
		{
			b.shift_right_1();
		}

		number temp = a;
		if (a.compare(b) == 1)
		{
			a = b;
			b = temp - b;
		}
		else
		{
			b = b - a;
		}
	}

	d = d * a;
	return d;
}

double_number number::lcm(number b)
{
	double_number lcm_val = this->double_mult(b) / (this->gcd(b)).to_double_number();
	return lcm_val;
}

number number::barrett_reduction(double_number x, number n, number mu)
{
	int k = n.high_word();
	double_number temp_x = x;
	temp_x.shift_words_right(k);
	double_number temp_q = temp_x.to_number().double_mult(mu);
	temp_q.shift_words_right(k + 2);
	number q = temp_q.to_number();

	// r = x - q * n
	double_number temp = q.double_mult(n);
	double_number r = x - temp;

	double_number n_dbl = n.to_double_number();
	while (r.compare(n_dbl) != -1)
	{
		r = r - n_dbl;
	}

	return r.to_number();
}

number number::pow_mod(number base, number exponent)
{
	number res("1");
	number mod = *this;
	int k = mod.high_word();

	double_number temp_mu;

	if (2 * (k + 1) < 129)
	{
		temp_mu._data[2 * (k + 1)] = 1;
	}
	else
	{
		cout << "Modulus too chonk for Mu." << endl;
		return number("0");
	}

	number mu = (temp_mu / mod.to_double_number()).to_number();

	int t = exponent.high_bit();
	if (t == -1)
	{
		return res;
	}

	for (int i = 0; i < t / 32; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			if ((exponent.words[i] & (uint32_t(1) << j)) != 0)
			{
				res = barrett_reduction(res.double_mult(base), mod, mu);
			}
			base = barrett_reduction(base.double_mult(base), mod, mu);
		}
	}

	for (int j = 0; j <= t % 32; j++)
	{
		if ((exponent.words[t / 32] & (uint32_t(1) << j)) != 0)
		{
			res = barrett_reduction(res.double_mult(base), mod, mu);
		}
		base = barrett_reduction(base.double_mult(base), mod, mu);
	}

	return res;
}

double_number number::double_mult(const number& other) const
{
	double_number res;

	for (int i = 0; i < 64; i++)
	{
		if (other.words[i] == 0) continue;

		uint64_t carry = 0;
		for (int j = 0; j < 64; j++)
		{
			uint64_t temp = (uint64_t)this->words[j] * other.words[i] + res._data[i + j] + carry;

			res._data[i + j] = (uint32_t)temp;
			carry = temp >> 32;
		}

		int k = i + 64;
		while (carry > 0 && k < 129)
		{
			uint64_t temp = (uint64_t)res._data[k] + carry;
			res._data[k] = (uint32_t)temp;
			carry = temp >> 32;
			k++;
		}
	}
	return res;
}

number double_number::to_number() const
{
	number res;
	for (int i = 0; i < 64; i++)
	{
		res.words[i] = this->_data[i];
	}
	return res;
}

double_number number::to_double_number() const
{
	double_number res;
	for (int i = 0; i < 64; i++)
	{
		res._data[i] = this->words[i];
	}
	return res;
}