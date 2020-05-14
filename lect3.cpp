#include <stdio.h>
#include <ctime>
#include <iostream>
#include <cilk/cilk.h>
#include <cilk/reducer_opadd.h>
#include <chrono>

using namespace std::chrono;
using namespace std;

const int MATRIX_SIZE = 1500;

void InitMatrix(double** matrix)
{
	for (int i = 0; i < MATRIX_SIZE; ++i)
	{
		matrix[i] = new double[MATRIX_SIZE + 1];
	}

	for (int i = 0; i < MATRIX_SIZE; ++i)
	{
		for (int j = 0; j <= MATRIX_SIZE; ++j)
		{
			matrix[i][j] = rand() % 2500 + 1;
		}
	}
}

void SerialGaussMethod(double **matrix, const int rows, double* result)
{
	int k;
	double koef;

	high_resolution_clock::time_point start;
	high_resolution_clock::time_point end;
	duration<double> duration;

	start = high_resolution_clock::now();
	for (k = 0; k < rows; ++k)
	{

		for (int i = k + 1; i < rows; ++i)
		{
			koef = -matrix[i][k] / matrix[k][k];

			for (int j = k; j <= rows; ++j)
			{
				matrix[i][j] += koef * matrix[k][j];
			}
		}

	}
	end = high_resolution_clock::now();
	duration = (end - start);
	cout << "Serial inner cycle duration : " << duration.count() << " seconds" << endl;
	result[rows - 1] = matrix[rows - 1][rows] / matrix[rows - 1][rows - 1];

	for (k = rows - 2; k >= 0; --k)
	{
		result[k] = matrix[k][rows];

		for (int j = k + 1; j < rows; ++j)
		{
			result[k] -= matrix[k][j] * result[j];
		}

		result[k] /= matrix[k][k];
	}
}

void  ParallelGauss(double **matrix, const int rows, double* result)
{
	int k;
	double koef;

	high_resolution_clock::time_point start;
	high_resolution_clock::time_point end;
	duration<double> duration;

	start = high_resolution_clock::now();
	for (k = 0; k < rows; ++k)
	{
		
		cilk_for (int i = k + 1; i < rows; ++i)
		{
			koef = -matrix[i][k] / matrix[k][k];

			for (int j = k; j <= rows; ++j)
			{
				matrix[i][j] += koef * matrix[k][j];
			}
		}

	}
	end = high_resolution_clock::now();
	duration = (end - start);
	cout << "Parallel inner cycle duration : " << duration.count() << " seconds" << endl;
	result[rows - 1] = matrix[rows - 1][rows] / matrix[rows - 1][rows - 1];

	for (k = rows - 2; k >= 0; --k)
	{
		result[k] = matrix[k][rows];

		for (int j = k + 1; j < rows; ++j)
		{
			result[k] -= matrix[k][j] * result[j];
		}

		result[k] /= matrix[k][k];
	}
}

void ParallelGaussJordon(double **matrix, const int rows, double* result)
{
	int k;
	double koef;

	for (k = 0; k < rows; ++k)
		cilk_for (int i = 0; i < rows; ++i)
		{
			if (i != k)
			{
				koef = -matrix[i][k] / matrix[k][k];
				for(int j = k; j <= rows; ++j)
					matrix[i][j] += koef * matrix[k][j];
			}
		}

	for (int i = 0; i < rows; ++i)
		result[i] = matrix[i][rows] / matrix[i][i];

}
int main()
{
	srand((unsigned)time(0));

	int i;

	const int test_matrix_lines = MATRIX_SIZE;

	double **test_matrix = new double*[test_matrix_lines];

	InitMatrix(test_matrix);
	double *result = new double[test_matrix_lines];

	SerialGaussMethod(test_matrix, test_matrix_lines, result);
	ParallelGauss(test_matrix, test_matrix_lines, result);


	for (i = 0; i < test_matrix_lines; ++i)
	{
		delete[]test_matrix[i];
	}
	/*
	cout << "Solution:" << endl;

	for (i = 0; i < test_matrix_lines; ++i)
	{
		printf("x(%d) = %lf\n", i, result[i]);
	}
	*/

	delete[] result;

	return 0;
}
