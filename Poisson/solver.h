#pragma once
#include <math.h>
#include <vector>


namespace u_real {
  double u(double x, double y);

  double u0y(double y);

  double u1y(double y);

  double ux0(double x);

  double ux1(double x);

  double f(double x, double y);
}

namespace u_kapkaev {
  double u(double x, double y);

  double u0y(double y);

  double u1y(double y);

  double ux0(double x);

  double ux1(double x);

  double f(double x, double y);
}

enum Functions {
  STRONGINA, KAPKAEV
};

class solver {

  double (*u0y)(double);
  double (*u1y)(double);
  double (*ux0)(double);
  double (*ux1)(double);
  double (*f)(double x, double y);
  double (*u)(double x, double y);

  void prepare(std::vector<std::vector<double>>& v, std::vector<std::vector<double>>& z);

  double step(std::vector<std::vector<double>>& v, std::vector<std::vector<double>>& z);

  void copy(std::vector<std::vector<double>>& v1, std::vector<std::vector<double>>& z1, std::vector<std::vector<double>>& v2, std::vector<std::vector<double>>& z2);

public:

  int N, M, max_it, it;
  double x0, X, y0, Y, k, h, hor, ver, A, max_z, epsilon;
  bool valid = 0;

  solver();

  solver(int function);



  void solve(int n, int m, double a, double b, double c, double d, double eps, int m_it, std::vector<std::vector<std::vector<double>>>& v, std::vector<std::vector<std::vector<double>>>& z);
};











