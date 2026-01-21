#pragma once
#include "number.hpp"

bool double_number::check_syntax(string n)
{
	regex hex_pattern("^[A-F0-9]+$");
	return regex_match(n, hex_pattern);
}

double_number::double_number(string s)
{
	int len = (int)s.size(); // Fixed C4267
	if (len > 1032)
	{
		cout << "Too big, even for double_number" << endl;
		return;
	}

	if (!check_syntax(s))
	{
		cout << "Strange syntax, check input" << endl;
		return;
	}

	// parsing hex string backwards
	for (int i = 0; i < _size; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			int idx = len - (i * 8 + j) - 1;
			if (idx < 0)
			{
				break;
			}

			if (int(s[idx]) < 58)
			{
				_data[i] += (s[idx] - 48) << (j * 4);
			}
			else
			{
				_data[i] += (s[idx] - 55) << (j * 4);
			}
		}
	}
	s.clear();
}

int double_number::get_highest_word() const
{
	for (int i = _size - 1; i > -1; i--)
	{
		if (_data[i] != 0)
		{
			return i;
		}
	}
	return -1;
}

int double_number::get_highest_bit() const
{
	for (int i = _size - 1; i > -1; i--)
	{
		if (_data[i] != 0)
		{
			for (int j = 31; j > -1; j--)
			{
				if (_data[i] & (1u << j))
				{
					return i * 32 + j;
				}
			}
		}
	}
	return -1;
}

void double_number::print_raw() const
{
	for (int i = 0; i < _size; i++)
	{
		cout << _data[i] << " ";
	}
	cout << endl;
}

void double_number::print_hex() const
{
	cout << endl;
	char hex_map[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	int k = get_highest_word();

	if (k >= 0)
	{
		for (int i = k; i > -1; i--)
		{
			uint32_t temp = _data[i];
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

int double_number::compare(double_number other) const
{
	int i = _size - 1;
	while (_data[i] == other._data[i])
	{
		i--;
		if (i == -1) return 0; // equal
	}

	if (_data[i] > other._data[i])
		return 1;
	else
		return -1;
}

void double_number::shift_words_left(int k)
{
	if (_size < k)
	{
		for (int i = 0; i < _size; i++) _data[i] = 0;
		return;
	}
	if (k < 0)
	{
		cout << "shift error: negative";
		return;
	}
	for (int i = _size - 1; i >= k; i--)
	{
		_data[i] = _data[i - k];
	}
	for (int i = k - 1; i >= 0; i--)
	{
		_data[i] = 0;
	}
}

void double_number::shift_words_right(int k)
{
	if (_size <= k)
	{
		for (int i = 0; i < _size; i++) _data[i] = 0;
		return;
	}

	for (int i = 0; i < _size - k; i++)
	{
		_data[i] = _data[i + k];
	}

	for (int i = _size - k; i < _size; i++)
	{
		_data[i] = 0;
	}
}

void double_number::shift_bits_left(int k)
{
	int top = this->get_highest_word();
	double_number temp_res;

	if (_size * _width < k)
	{
		for (int i = 0; i < _size; i++) _data[i] = 0;
		return;
	}
	if (k < 0)
	{
		cout << "error";
		return;
	}

	int word_shift = k / _width;
	int bit_shift = k % _width;

	if (bit_shift == 0)
	{
		this->shift_words_left(k / _width);
		return;
	}
	else
	{
		for (int i = 0; i <= top; i++)
		{
			if (i + word_shift < _size)
			{
				temp_res._data[i + word_shift] |= (_data[i] << bit_shift);
			}
			if (i + word_shift + 1 < _size)
			{
				temp_res._data[i + word_shift + 1] |= (_data[i] >> (_width - bit_shift));
			}
		}
	}
	*this = temp_res;
}


bool double_number:: operator==(const double_number& other) const
{
	return (this->compare(other) == 0);
}

double_number double_number::operator+(const double_number& other)
{
	double_number res("0");
	uint64_t carry = 0;
	for (int i = 0; i < _size; i++)
	{
		uint64_t temp = uint64_t(this->_data[i]) + uint64_t(other._data[i]) + carry;
		res._data[i] = (uint32_t)(temp & ((uint32_t)-1));
		carry = temp >> _width;
	}
	res._carry = carry;
	return res;
}

double_number double_number::operator+(const uint32_t& k)
{
	double_number res;
	uint64_t carry = 0;
	uint64_t temp = uint64_t(this->_data[0]) + uint64_t(k);
	res._data[0] = (uint32_t)(temp & ((uint32_t)-1));
	carry = temp >> _width;

	for (int i = 1; i < _size; i++)
	{
		if (carry == 0)
		{
			for (int j = i; j < double_number::_size; j++)
			{
				res._data[j] = this->_data[j];
			}
			break;
		}
		temp = uint64_t(this->_data[i]) + carry;
		res._data[i] = (uint32_t)(temp & ((uint32_t)-1));
		carry = temp >> _width;
	}
	res._carry = carry;
	return res;
}

double_number double_number::operator-(const double_number& other)
{
	double_number res;

	if (this->compare(other) == -1)
	{
		double_number max_val("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
		int64_t borrow = 0;
		for (int i = 0; i < _size; i++)
		{
			int64_t temp = int64_t(other._data[i]) - int64_t(_data[i]) - borrow;

			if (temp >= 0)
			{
				res._data[i] = (uint32_t)temp; // Cast
				borrow = 0;
			}
			else
			{
				res._data[i] = (uint32_t)(temp + (uint64_t(1) << _width)); // Cast
				borrow = 1;
			}
		}
		res = max_val - res;
		res = res + 1;
		return res;
	}

	int64_t borrow = 0;
	for (int i = 0; i < _size; i++)
	{
		int64_t temp = int64_t(_data[i]) - int64_t(other._data[i]) - borrow;

		if (temp >= 0)
		{
			res._data[i] = (uint32_t)temp; // Cast
			borrow = 0;
		}
		else
		{
			res._data[i] = (uint32_t)(temp + (uint64_t(1) << _width)); // Cast
			borrow = 1;
		}
	}
	return res;
}

double_number double_number::operator*(const double_number& other)
{
	double_number res;
	for (int i = 0; i < _size; i++)
	{
		double_number temp = this->mul_scalar(other._data[i]);
		temp.shift_words_left(i);
		res = res + temp;
	}
	return res;
}

double_number double_number::mul_scalar(uint32_t k) const
{
	double_number res;
	uint32_t carry = 0;
	for (int i = 0; i < _size; i++)
	{
		uint64_t temp = (uint64_t(_data[i]) * k) + carry;
		res._data[i] = (uint32_t)(temp & ((uint32_t)-1));
		carry = (uint32_t)(temp >> _width);
	}
	res._carry = carry;
	return res;
}

double_number double_number::operator/(const double_number& other)
{
	double_number res;
	int k = other.get_highest_bit();
	double_number remainder = *this;
	double_number quotient;

	while (remainder.compare(other) != -1)
	{
		int t = remainder.get_highest_bit();
		res = other;
		res.shift_bits_left(t - k);

		if (remainder.compare(res) == -1)
		{
			t--;
			res = other;
			res.shift_bits_left(t - k);
		}

		double_number temp_one("1");
		temp_one.shift_bits_left(t - k);
		remainder = remainder - res;
		quotient = quotient + temp_one;
	}
	return quotient;
}

double_number double_number::operator<<(int k)
{
	int top = this->get_highest_word();
	double_number temp_res;

	if (_size * _width < k)
	{
		return temp_res;
	}

	if (k < 0)
	{
		cout << "Error: Negative shift amount." << endl;
		return *this;
	}

	int word_shift = k / _width;
	int b_shift = k % _width;

	if (b_shift == 0)
	{
		this->shift_words_left(word_shift);
		return *this;
	}
	else
	{
		for (int i = 0; i <= top; i++)
		{
			if (i + word_shift < _size)
			{
				temp_res._data[i + word_shift] |= (_data[i] << b_shift);
			}

			if (i + word_shift + 1 < _size)
			{
				temp_res._data[i + word_shift + 1] |= (_data[i] >> (_width - b_shift));
			}
		}
	}

	*this = temp_res;
	return *this;
}

double_number double_number::operator>>(int k)
{
	int top = this->get_highest_word();
	if (top == -1)
	{
		return *this;
	}

	double_number temp_res;

	if (k < 0)
	{
		cout << "Error: Negative shift amount." << endl;
		return *this;
	}

	if (_size * _width <= k)
	{
		return temp_res;
	}

	int word_shift = k / _width;
	int b_shift = k % _width;

	if (b_shift == 0)
	{
		this->shift_words_right(word_shift);
		return *this;
	}
	else
	{
		for (int i = top; i >= 0; i--)
		{
			// shifting right part
			if (i - word_shift >= 0)
			{
				temp_res._data[i - word_shift] |= (_data[i] >> b_shift);
			}

			// shifting overflow part
			if (i - word_shift - 1 >= 0)
			{
				temp_res._data[i - word_shift - 1] |= (_data[i] << (_width - b_shift));
			}
		}
	}

	*this = temp_res;
	return *this;
}