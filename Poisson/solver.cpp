#include "solver.h"



double u_real::u(double x, double y) {
	return pow(x, 2.0) + pow(y, 3.0) + 1;
}

double u_real::u0y(double y) {
	return pow(y, 3.0) + 1;
}

double u_real::u1y(double y) {
	return pow(y, 3.0) + 2;
}

double u_real::ux0(double x) {
	return pow(x, 2.0) + 1;
}

double u_real::ux1(double x) {
	return pow(x, 2.0) + 2;
}

double u_real::f(double x, double y) {
	return -6 * y - 2;
}


solver::solver() {
	k = 0;
	h = 0;
	N = 0;
	M = 0;
	x0 = 0;
	X = 0;
	y0 = 0;
	Y = 0;
	u0y = u_real::u0y;
	u1y = u_real::u1y;
	ux0 = u_real::ux0;
	ux1 = u_real::ux1;
	u = u_real::u;
	f = u_real::f;
}

solver::solver(double (*_0y)(double), double (*_1y)(double), double (*_x0)(double), double (*_x1)(double), double (*_f)(double x, double y), double (*_u)(double x, double y)) {
	k = 0;
	h = 0;
	N = 0;
	M = 0;
	x0 = 0;
	X = 0;
	y0 = 0;
	Y = 0;

	u0y = _0y;
	u1y = _1y;
	ux0 = _x0;
	ux1 = _x1;
	u = _u;
	f = _f;
}

void solver::prepare(std::vector<std::vector<double>>& v, std::vector<std::vector<double>>& z) {

	//map[x][y] 

	v.resize(N + 1);
	for (int i = 0; i < N + 1; i++)
		v[i].resize(M + 1);

	z.resize(N + 1);
	for (int i = 0; i < N + 1; i++)
		z[i].resize(M + 1);


	for (int i = 0; i < N + 1; i++) {
		v[i][0] = ux0(h * i);
		v[i][M] = ux1(h * i);
	}

	for (int i = 1; i < M; i++) {
		v[0][i] = u0y(k * i);
		v[M][i] = u1y(k * i);
	}

	for (int i = 1; i < N; i++)
		for (int j = 1; j < M; j++)
			v[i][j] = 0;

	for (int i = 0; i < N + 1; i++)
		for (int j = 0; j < M + 1; j++)
			z[i][j] = 0;
}

double solver::step(std::vector<std::vector<double>>& v, std::vector<std::vector<double>>& z) {
	//zeidel

	it++;
	double max_z = 0;
	for (int i = 1; i < N; i++)
		for (int j = 1; j < M; j++) {
			v[i][j] = (-f(h * i, k * j) - hor * (v[i - 1][j] + v[i + 1][j]) - ver * (v[i][j - 1] + v[i][j + 1])) / A;
			z[i][j] = v[i][j] - u(i * h, j * k);
			if (z[i][j] > max_z)
				max_z = z[i][j];
		}

	return max_z;

}

void solver::copy(std::vector<std::vector<double>>& v1, std::vector<std::vector<double>>& z1, std::vector<std::vector<double>>& v2, std::vector<std::vector<double>>& z2) {
	for (int i = 1; i < N; i++)
		for (int j = 1; j < M; j++) {
			v2[i][j] = v1[i][j];
			z2[i][j] = z1[i][j];
		}
}

void solver::solve(int n, int m, double a, double b, double c, double d, double eps, int m_it, std::vector<std::vector<std::vector<double>>>& v, std::vector<std::vector<std::vector<double>>>& z) {
	N = n;
	M = m;
	x0 = a;
	X = b;
	y0 = c;
	Y = d;
	k = (Y - y0) / M;
	h = (X - x0) / N;

	//std::vector<bool> valid(9);
	//for (int i = 0; i < 9; i++)
	//	valid[i] = false;
	max_it = m_it;
	it = 0;
	hor = 1.0 / pow(h, 2.0);
	ver = 1.0 / pow(k, 2.0);
	A = -2.0 * (1.0 / pow(h, 2) + 1.0 / pow(k, 2));
	v.resize(10);
	z.resize(10);
	for (int i = 0; i < 10; i++)
		prepare(v[i], z[i]);
	double cur_mz;
	//double Z = (cur_mz - eps);
	int cur_photo = 0;
	for (int i = 0; i < max_it; i++) {
		cur_mz = step(v[9], z[9]);
		if (i == max_it / 10 * cur_photo && cur_photo < 9) {
			copy(v[9], z[9], v[cur_photo], z[cur_photo]);
			cur_photo++;
		}
	}
}

