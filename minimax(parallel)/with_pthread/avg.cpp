#include <iostream>
#include <fstream>

int main (int argc, char **argv)
{
    std::fstream in, out;
    in.open (argv [1], std::ios::in);
    out.open ("log", std::ios::out | std::ios::app);

    int num, tmp1;
    double sum = 0.0, tmp2;
    while (!in.eof ())
    {
        in >> tmp1 >> tmp2;
        sum += tmp2;
        num++;
    }

    out << argv [1] << "\t" << sum / (double) num << std::endl;

    in.close ();
    out.close ();
}