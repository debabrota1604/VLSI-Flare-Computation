#include<iostream>
#include<math.h>
using namespace std;

double *terms;
double Guassian(int x, double sigma)
{
        double c = 2.0 * sigma * sigma;
        return pow(M_E, -x * x / c) /sqrt(c * M_PI);
}
void GuassianTerms(int kernalSize, double sigma)
{
        terms = new double[kernalSize];
        for (int i = 0; i < kernalSize; ++i) {
                terms[i] = Guassian(i - kernalSize / 2, sigma);
        }
}
int Main()
{
        double *terms = GuassianTerms(5, 1);
        double sum = terms.Aggregate((t1, t2) => t1 + t2); // aggregate to normalise result
        cout << "\r\n" << terms.Select(i => (i / sum).ToString("0.00000"))));
}
