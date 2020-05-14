#include <cilk/cilk.h>
#include <iostream>
#include <vector>
#include <cilk/cilk_api.h>
#include <cilk/reducer_max.h>
#include <cilk/reducer_min.h>
#include <cilk/reducer_vector.h>
#include <chrono>

using namespace std::chrono;
using namespace std;

void ReducerMinTest(int *mass_pointer, const long size)
{
	cilk::reducer<cilk::op_min_index<long, int>> minimum;
	cilk_for(long i = 0; i < size; ++i)
	{
		minimum->calc_min(i, mass_pointer[i]);
	}
	printf("Minimal element = %d has index = %d\n",
		minimum->get_reference(), minimum->get_index_reference());
}

void ParallelSort(int *begin, int *end)
{
	if (begin != end)
	{
		--end;
		int *middle = std::partition(begin, end, std::bind2nd(std::less<int>(), *end));
		std::swap(*end, *middle);
		cilk_spawn ParallelSort(begin, middle);
		ParallelSort(++middle, ++end);
		cilk_sync;

	}
}

void CompareForAndCilk_For(size_t sz)
{
	high_resolution_clock::time_point start;
	high_resolution_clock::time_point end;
	cilk::reducer<cilk::op_vector<int>>red_vec;
	vector<int> vec;
	duration<double> duration;
	
	cout << "Comparing for and cilk_for for size " << sz << endl;
	//parallel
	start = high_resolution_clock::now();
	cilk_for(size_t i = 0; i < sz; ++i)
		red_vec->push_back(rand() % 20000 + 1);
	end = high_resolution_clock::now();
	duration = (end - start);
	cout << "Parallel duration is: " << duration.count() << " seconds" << endl;

	//sync
	start = high_resolution_clock::now();
	for (size_t i = 0; i < sz; ++i)
		vec.push_back(rand() % 20000 + 1);
	end = high_resolution_clock::now();
	duration = (end - start);
	cout << "Sync     duration is: " << duration.count() << " seconds" << endl;
	cout << endl;
}

int main()
{
	srand((unsigned)time(0));

	__cilkrts_set_param("nworkers", "4");

	long i;
	high_resolution_clock::time_point start;
	high_resolution_clock::time_point end;
	duration<double> duration;


	int ms_size[4] = { 10000, 100000, 500000, 1000000 };

	for (long mass_size : ms_size)
	{
		cout << "Testing sort for size " << mass_size << endl;
		int *mass_begin, *mass_end;
		int *mass = new int[mass_size];

		for (i = 0; i < mass_size; ++i)
		{
			mass[i] = (rand() % 25000) + 1;
		}

		mass_begin = mass;
		mass_end = mass_begin + mass_size;
		ReducerMinTest(mass, mass_size);

		start = high_resolution_clock::now();
		ParallelSort(mass_begin, mass_end);
		end = high_resolution_clock::now();
		duration = (end - start);
		cout << "Sort duration is: " << duration.count() << " seconds" << endl;

		ReducerMinTest(mass, mass_size);

		cout << endl;
		delete[]mass;
	}

	int size[8] = { 1000000, 100000, 10000, 1000, 500, 100, 50, 10 };
	for (int sz : size)
		CompareForAndCilk_For(sz);

	return 0;
}
