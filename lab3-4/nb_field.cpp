#include "nb_field.hpp"

// Placeholder table. Real table for m=131 and i Dont have time to work with that.
// I suppose I can fix this or create auto-gen but idm fr.
// Filled with 0 just so it links.
const int nb_field::mult_table[265] = { 0 };

bool nb_field::check_syntax(string n)
{
    regex bin_regex("^[01]+$");
    return regex_match(n, bin_regex);
}

nb_field::nb_field(string s)
{
    data[0] = data[1] = data[2] = 0;

    int len = s.size();

    if (len > 131)
    {
        cout << "Error: Too big for NB (max 131)" << endl;
        return;
    }

    if (!check_syntax(s))
    {
        cout << "Error: Syntax broken" << endl;
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

nb_field nb_field::operator+(const nb_field& other) const
{
    nb_field res;

    for (int i = 0; i < 3; i++)
    {
        res.data[i] = this->data[i] ^ other.data[i];
    }
    return res;
}

bool nb_field::mult_helper(int i, const uint64_t u[3], const uint64_t v[3]) const
{

    int j_0 = mult_table[0] + i;
    if (j_0 >= 131) j_0 -= 131;

    uint64_t v_temp[3] = { 0 };

    v_temp[0] |= (v[j_0 >> 6] >> (j_0 & 63)) & 1;

    int i_curr = 1;

    // Loop limit adjusted for table size approx
    for (int k = 1; k < 262; )
    {
        int j_1 = mult_table[k] + i;
        int j_2 = mult_table[k + 1] + i;

        if (j_1 >= 131) j_1 -= 131;
        if (j_2 >= 131) j_2 -= 131;

        uint64_t b_1 = (v[j_1 >> 6] >> (j_1 & 63)) & 1;
        uint64_t b_2 = (v[j_2 >> 6] >> (j_2 & 63)) & 1;

        // putting bits into temp vector
        v_temp[i_curr >> 6] |= ((b_1 ^ b_2) << (i_curr & 63));

        k += 2;
        i_curr++;
    }

    bool res_bit = 0;

    for (int j = 0; j < 131; j++)
    {
        int j_shifted = i + j;
        if (j_shifted >= 131) j_shifted -= 131;

        // complex dot product logic
        res_bit ^= ((u[j_shifted >> 6] >> (j_shifted & 63)) & 1) &
            ((v_temp[j >> 6] >> (j & 63)) & 1);
    }
    return res_bit;
}

nb_field nb_field::operator*(const nb_field& other) const
{
    nb_field res;

    // m = 131 loop
    for (int i = 130; i > -1; i--)
    {
        if (mult_helper(i, this->data, other.data))
        {
            res.data[i / 64] |= (uint64_t(1) << (i % 64));
        }
    }
    return res;
}

nb_field nb_field::square() const
{
    nb_field res;

    bool last_bit = (this->data[0] & 1);

    res.data[0] = (this->data[0] >> 1) | (this->data[1] << 63);
    res.data[1] = (this->data[1] >> 1) | (this->data[2] << 63);
    res.data[2] = (this->data[2] >> 1);

    if (last_bit) res.data[2] |= (uint64_t(1) << (131 % 64 - 1)); // set top bit

    return res;
}

void nb_field::print_bin() const
{
    cout << endl;
    int k = get_highest_bit();

    if (k < 0)
    {
        cout << "0" << endl;
        return;
    }

    bool printing = false;

    for (int i = k; i >= 0; i--)
    {
        uint64_t val = data[i];
        for (int j = 63; j >= 0; j--)
        {
            bool bit = (val >> j) & 1;
            if (i == k)
            {
                if (bit) { printing = true; cout << "1"; }
                else if (printing) cout << "0";
            }
            else cout << bit;
        }
    }
    cout << endl;
}

int nb_field::get_highest_bit() const
{
    for (int i = 2; i >= 0; i--) // 3 chunks
    {
        if (data[i] != 0) return i;
    }
    return -1;
}

int nb_field::trace() const
{
    int weight = 0;
    for (int i = 0; i < 131; i++)
    {
        if ((data[i / 64] & (uint64_t(1) << (i % 64))) != 0)
        {
            weight++;
        }
    }
    return weight & 1;
}

nb_field nb_field::square_times(int k) const
{
    nb_field temp = *this;
    for (int j = 0; j < k; j++)
    {
        temp = temp.square();
    }
    return temp;
}

nb_field nb_field::inverse() const
{

    nb_field temp = *this;

    nb_field res;

    return temp;
}

nb_field nb_field::power(const nb_field& exp) const
{

    nb_field res;
  
    return res;
}