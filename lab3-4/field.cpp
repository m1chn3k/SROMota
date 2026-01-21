#include "field.hpp"

bool field::check_syntax(string n)
{
    regex bin_pattern("^[01]+$");
    return regex_match(n, bin_pattern);
}

field::field(string s) {
    for (int i = 0; i < num_words; i++) data[i] = 0;

    int len = s.size();

    // Variant 9: m = 283
    if (len > 283)
    {
        cout << "Error: Number is too chonky (max 283 bits)" << endl;
        return;
    }

    if (!check_syntax(s))
    {
        cout << "Error: Binary only please" << endl;
        return;
    }

    for (int i = 0; i < len; i++)
    {
        if (s[len - 1 - i] == '1')
        {
            data[i / 64] |= (uint64_t(1) << (i % 64));
        }
    }
}

field field::operator+(const field& other) const
{
    field res;
    // XORing all chunks
    for (int i = 0; i < num_words; i++)
    {
        res.data[i] = this->data[i] ^ other.data[i];
    }
    return res;
}

field field::operator*(const field& other) const
{
    field res;
    field temp = *this;

 
    for (int i = 0; i < 283; i++)
    {
        if ((other.data[i / 64] & (uint64_t(1) << (i % 64))) != 0)
        {
            res = res + temp;
        }
        temp.shift_once();
    }
    return res;
}

void field::shift_once()
{

    bool overflow = (this->data[4] & (uint64_t(1) << 26)) != 0;

    for (int i = num_words - 1; i > 0; i--) {
        this->data[i] = (this->data[i] << 1) | (this->data[i - 1] >> 63);
    }
    this->data[0] = this->data[0] << 1;


    if (overflow)
    {
        this->data[4] &= ~(uint64_t(1) << 27);

        this->data[0] ^= (uint64_t(1) << 26);
        this->data[0] ^= (uint64_t(1) << 9);
        this->data[0] ^= (uint64_t(1) << 1);
        this->data[0] ^= 1; // x^0
    }
}

void field::print_bin() const
{
    cout << endl;
    int k = get_highest_bit();

    if (k < 0)
    {
        cout << "0" << endl;
        return;
    }

    bool start_printing = false;

    for (int i = k; i >= 0; i--)
    {
        uint64_t val = data[i];

     
        int bit_limit = (i == 4) ? 26 : 63; // optimization: don't check unused bits in top word

        for (int j = 63; j >= 0; j--)
        {
            bool bit = (val >> j) & 1;

            if (i == k)
            {
                if (bit)
                {
                    start_printing = true;
                    cout << "1";
                }
                else if (start_printing)
                {
                    cout << "0";
                }
            }
            else
            {
                cout << bit;
            }
        }
    }
    cout << endl;
}

int field::get_highest_bit() const
{
    for (int i = num_words - 1; i >= 0; i--)
    {
        if (data[i] != 0) return i;
    }
    return -1;
}

field field::square() const
{
    field res;
    field temp;
    temp.data[0] = 1; // set to 1

    for (int i = 0; i < 283; i++)
    {
        if ((this->data[i / 64] & (uint64_t(1) << (i % 64))) != 0)
        {
            res = res + temp;
        }
        // square is shift twice
        temp.shift_once();
        temp.shift_once();
    }
    return res;
}

field field::power(const field& exp) const
{
    field res;
    res.data[0] = 1; // result = 1

    // standard square-and-multiply, scanning from MSB
    // 282 is max index for 283 bits
    for (int i = 282; i > -1; i--)
    {
        res = res.square();
        if ((exp.data[i / 64] & (uint64_t(1) << (i % 64))) != 0)
        {
            res = res * *this;
        }
    }
    return res;
}

int field::trace() const
{
    field temp = *this;
    field accumulator = *this;


    for (int i = 1; i < 283; i++)
    {
        temp = temp.square();
        accumulator = accumulator + temp;
    }
    return accumulator.data[0] & 1; 
}

field field::inverse() const
{

    string ones = "";
    for (int i = 0; i < 282; i++) ones += "1";
    ones += "0";

    return this->power(field(ones));
}