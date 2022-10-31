#include <cmath>
#include <vector>
#include <iostream>
#include <iomanip>

double gaussian (double x, double mu, double sigma) {
  return std::exp( -(((x-mu)/(sigma))*((x-mu)/(sigma)))/2.0 );
}

typedef std::vector<double> kernel_row;
typedef std::vector<kernel_row> kernel_type;

kernel_type produce2dGaussianKernel (int kernelRadius) {
  double sigma = 1; //kernelRadius/2.;
  
  kernel_type kernel2d(kernelRadius, kernel_row(kernelRadius));
  
  double sum = 0;
  // compute values
  for (int row = 0; row < kernel2d.size(); row++)
  {
	double row_mean = kernel2d.size()/2;
    for (int col = 0; col < kernel2d[row].size(); col++)
    {
		double col_mean = kernel2d[row].size()/2;
		/*double x = gaussian(row, kernelRadius, sigma) * gaussian(col, kernelRadius, sigma);*/
		kernel2d[row][col] = exp( -0.5 * (pow((row-row_mean)/sigma, 2.0) + pow((col-col_mean)/sigma,2.0)) ) / (M_PI * pow(sigma,2) );
		//kernel2d[row][col] = x;
		sum += kernel2d[row][col];
    }
   }
       
  // normalize
  for (int row = 0; row < kernel2d.size(); row++)
    for (int col = 0; col < kernel2d[row].size(); col++)
      kernel2d[row][col] /= sum;
      
  return kernel2d;
}

int main() {
	int size;
	std::cout << "Set kernel size: ";
	std::cin >> size;
  kernel_type kernel2d = produce2dGaussianKernel(size);
  
  std::cout << std::setprecision(5) << std::fixed;
    std::cout << '\n';
    std::cout << '\n';
    
  for (int row = 0; row < kernel2d.size(); row++) {
    for (int col = 0; col < kernel2d[row].size(); col++)
      std::cout << kernel2d[row][col] << "\t\t";
	  std::cout << '\n';
    
  }
}
