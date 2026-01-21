#include "nb_field.hpp"
#include "field.hpp"

int make_positive(int a, int n) {
    return (a % n + n) % n;
}

int main()
{
    cout << "--- POLYNOMIAL BASIS (m=283) ---" << endl;
    field A("10110010100111010100010110101001011100100101101010100011101001011001010101110010010101001011000110101001011101010");
    field B("10011010110001010101110010010110101010001101001001110101010010110010101000110101010011100100101011010100010110100");

    field sum = A + B;
    field prod = A * A;
    field sq = A.square();

    cout << "A + B: "; sum.print_bin();
    cout << "A * A: "; prod.print_bin();
    cout << "A^2:   "; sq.print_bin();
    cout << "Trace: " << A.trace() << endl;

    cout << endl << "--- NORMAL BASIS (m=131) ---" << endl;
    nb_field NA("101100101001110101000101101010010111001001011010101000111");
    nb_field NB("100110101100010101011100100101101010100011010010011101010");

    nb_field n_sum = NA + NB;

    cout << "NA + NB: "; n_sum.print_bin();

    cout << "NA * NA (Needs valid L table): ";
    (NA * NA).print_bin();

    cout << "Trace(NA): " << NA.trace() << endl;

    return 0;
}