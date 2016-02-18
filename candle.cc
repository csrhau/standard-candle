#include <getopt.h>
#include <iostream>
#include <cstdlib>
#include <cmath>

#define MATRIX_SZ 500

#define GETOPTS "i:s:m:"
static char *progname;
static struct option long_opts[] = {
  {"iterations", required_argument, NULL, 'i'},
  {"sqrt-cycles", required_argument, NULL, 's'},
  {"matmul-cycles", required_argument, NULL, 'm'},
  {NULL, 0, NULL, 0}
};

void print_usage() {
  std::cerr << progname << ": a program to generate detectable fluctuations in power output\n"
            << "Usage: " << progname << " [OPTIONS]\n"
            << "-i --iterations         Outer loop iterations\n"
            << "-s --sqrt-cycles        Low power (sqrt) kernel iterations\n"
            << "-m --matmul-cycles      High power (matrix multiplication) kernel iterations"
            << std::endl;
}

double matmul_kern(int cycles) {
  double *a = new double[MATRIX_SZ * MATRIX_SZ];
  double *b = new double[MATRIX_SZ * MATRIX_SZ];
  double *c = new double[MATRIX_SZ * MATRIX_SZ];
  // Initialize numbers to small value
  for (int i = 0; i < MATRIX_SZ * MATRIX_SZ; ++i) {
    a[i] = double(i) / (double(i)+1.0);
    b[i] = double(i) / (double(i)+1.0);
  }
  for (int cyc = 0; cyc < cycles; ++cyc) {
    for (int i = 0; i < MATRIX_SZ; ++i) {
      for (int j = 0; j < MATRIX_SZ; ++j) {
        double acc = 0;
        for (int k = 0; k < MATRIX_SZ; ++k) {
          acc += a[i * MATRIX_SZ + k] + b[k * MATRIX_SZ + j];
        }
        c[i*MATRIX_SZ+j] = acc;
      }
    }
    int nonce = cyc % (MATRIX_SZ * MATRIX_SZ);
    a[nonce] = c[nonce] * 0.1 * b[nonce] + 1.0;
    b[nonce] = c[nonce] / a[nonce] * 0.1;
  }
  double temp = a[0];
  delete [] a;
  delete [] b;
  delete [] c;
  return temp;
}

double sqrt_kern(int cycles) {
  double *a = new double[MATRIX_SZ * MATRIX_SZ];
  for (int i = 0; i < MATRIX_SZ * MATRIX_SZ; ++i) {
    a[i] = double(i * cycles) + 42.32421;
  }
  for (int cyc = 0; cyc < cycles; ++cyc) {
    for (int i = 0; i < MATRIX_SZ * MATRIX_SZ - 1; ++i) {
      a[i] = sqrt(a[i + 1]);
    }
    int nonce = cyc % (MATRIX_SZ * MATRIX_SZ);
    a[nonce] = a[MATRIX_SZ * MATRIX_SZ - nonce - 1];
  }
  double temp = a[0];
  delete [] a;
  return temp;
}


int main(int argc, char *argv[]) {
  progname = argv[0];
  int optc;
  bool show_usage = false;
  int iterations = 10;
  int sqrt_cycs = 1500;
  int matmul_cycs = 2;
  while ((optc = getopt_long(argc, argv, GETOPTS, long_opts, NULL)) != -1) {
    switch (optc) {
      case 'i':
        iterations = atoi(optarg);
        break;
      case 's':
        sqrt_cycs = atoi(optarg);
        break;
      case 'f':
        matmul_cycs = atoi(optarg);
        break;
      default:
          show_usage = 1;
    }
  }
  if (show_usage || iterations < 1 || sqrt_cycs < 1 || matmul_cycs < 1 || optind < argc) {
    print_usage();
    return EXIT_FAILURE;
  }

  std::cout << progname << " invoked\n"
            << "Performing " << iterations << " iterations consisting of:\n"
            << "\t" << sqrt_cycs << " cycles of the integer sqrt kernel; and\n"
            << "\t" << matmul_cycs << " iterations of naiive matrix multiplication"
            << std::endl;

  for (int i = 0; i < iterations; ++i) {
    std::cout << "Cycle " << i << std::endl;
    std::cout << matmul_kern(matmul_cycs) << std::endl;
    std::cout << sqrt_kern(sqrt_cycs) << std::endl;
  }

  return EXIT_SUCCESS;
}
