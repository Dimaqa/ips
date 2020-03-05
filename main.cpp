#include <math.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <future>

using namespace std;
using namespace std::chrono;

# define M_PI           3.14159265358979323846 

double func(double x)
{
	return 4 / sqrt(4 - pow(x, 2));
}

double right_square_integral(int accuracy, double a, double b)
{
	//init values
	double integral = 0;
	double step = (b - a) / double(accuracy);

	//start timer
	auto start = high_resolution_clock::now();

	//main cycle
	for (int i = 0; i < accuracy; i++)
		integral += func(a + double(i) / double(accuracy)) * step;

	//end time
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);

	//print output
	cout << "RIGHT : " << integral;
	cout << ". Time taken by function: "
		<< duration.count() << " microseconds" << endl;
	return integral;
}

void  threading_right_square_integral(double *integral, double int_step, double a, double b)
{
	for (double i = a + int_step; i <= b; i += int_step)
		*integral += func(i) * int_step;
}

double threadint_integral(int accuracy, double a, double b)
{
	int n = 4;
	double integral = 0;
	double step = (b - a) / n;
	double int_step = (b - a) / double(accuracy);
	vector<thread> threads(n);
	vector<double> integrals(n);

	// initialize threads
	auto start = high_resolution_clock::now();
	for (int i = 0; i < n; i++)
		threads[i] = thread(threading_right_square_integral, &integrals[i], int_step, a + i * step, a + (i + 1) * step);
	
	//wait for threads finish and sum up results
	for (int i = 0; i < n; i++)
	{
		threads[i].join();
		integral += integrals[i];
	}

	// print time, return results
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	cout << "Value: " << integral;
	cout << ". time taken by function: "
		<< duration.count() << " microseconds" << endl;
	return integral;
	
}
int main()
{
	double a = 0;
	double b = 1;
	int accuracy[5] = { 100, 1000, 10000, 100000, 1000000 };
	double teoretical = M_PI * 2 / 3;
	for (int acc : accuracy)
	{
		cout << "ACCURACY : " << acc << endl;
		cout << "DIFF : " << abs(teoretical - threadint_integral(acc, a, b)) << endl;
		cout << "DIFF : " << abs(teoretical - right_square_integral(acc, a, b)) << endl;
		cout << endl;
	}
	return 0;
}
