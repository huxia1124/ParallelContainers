// ParallelContainers.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "STXUtility.h"

#include <iostream>
#include <omp.h>
#include <set>
#include <chrono>
#include <atomic>
#include <concurrent_unordered_set.h>


const int count = 200000;		//Number of test elements
const int thread_count = 8;		//Number of parallel threads to run the test cases

void test_std_set_insert(std::set<int> &intSet)
{
	//////////////////////////////////////////////////////////////////////////
	//STL set
	//Insertion
	auto begin = std::chrono::high_resolution_clock::now();
#pragma omp parallel
	{
		int t = omp_get_thread_num();
		for (int i = count * t; i < count * (t + 1); i++)
		{
#pragma omp critical
			intSet.insert(i);
		}
	}

#pragma 
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Insertion:       \t" << intSet.size() << " items! " << std::endl;
	std::cout << "Insertion:       " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms elapsed." << std::endl;
}

void test_std_set_searching(std::set<int> &intSet)
{
	//////////////////////////////////////////////////////////////////////////
	//STL set
	//Searching
	auto begin = std::chrono::high_resolution_clock::now();
	std::atomic<int> totalFoundAll = 0;
#pragma omp parallel
	{
		int totalFound = 0;
		int t = omp_get_thread_num();
		for (int i = count * t; i < count * (t + 1); i++)
		{
#pragma omp critical
			{
				auto found = intSet.find(((rand() * rand() + rand()) % count) + count * t);
				if (found != intSet.end())
					totalFound++;
			}
		}
		totalFoundAll += totalFound;
	}
	std::cout << "Found:           \t" << totalFoundAll << " items." << std::endl;

#pragma 
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Searching:       " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms elapsed." << std::endl;
}

void test_std_set_erasing(std::set<int> &intSet)
{
	//////////////////////////////////////////////////////////////////////////
	//STL set
	//Searching
	auto begin = std::chrono::high_resolution_clock::now();
	std::atomic<int> totalFoundAll = 0;
#pragma omp parallel
	{
		int totalFound = 0;
		int t = omp_get_thread_num();
		for (int i = count * t; i < count * (t + 1); i++)
		{
#pragma omp critical
			intSet.erase(i);
		}
	}
#pragma 
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Erasing:         " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms elapsed." << std::endl;
	std::cout << "Now:             \t" << intSet.size() << " items! " << std::endl;
}

void test_concurrent_set_insert(Concurrency::concurrent_unordered_set<int> &intSet)
{
	//////////////////////////////////////////////////////////////////////////
	//
	//Insertion
	auto begin = std::chrono::high_resolution_clock::now();
#pragma omp parallel
	{
		int t = omp_get_thread_num();
		for (int i = count * t; i < count * (t + 1); i++)
			intSet.insert(i);
	}

#pragma 
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Insertion:       \t" << intSet.size() << " items! " << std::endl;
	std::cout << "Insertion:       " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms elapsed." << std::endl;

}

void test_concurrent_set_searching(Concurrency::concurrent_unordered_set<int> &intSet)
{
	//////////////////////////////////////////////////////////////////////////
	//CSTXHashSet set
	//Searching
	auto begin = std::chrono::high_resolution_clock::now();
	std::atomic<int> totalFoundAll = 0;
#pragma omp parallel
	{
		int totalFound = 0;
		int t = omp_get_thread_num();
		for (int i = count * t; i < count * (t + 1); i++)
		{
			int targetValue = ((rand() * rand() + rand()) % count) + count * t;
			auto found = intSet.find(targetValue);
			if (found != intSet.end())
				totalFound++;
		}
		totalFoundAll += totalFound;
	}
	std::cout << "Found:           \t" << totalFoundAll << " items." << std::endl;

#pragma 
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Searching:       " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms elapsed." << std::endl;
}

void test_concurrent_set_erasing(Concurrency::concurrent_unordered_set<int> &intSet)
{
	//////////////////////////////////////////////////////////////////////////
	//CSTXHashSet set
	//Searching
	auto begin = std::chrono::high_resolution_clock::now();
	std::atomic<int> totalFoundAll = 0;
#pragma omp parallel
	{
		int totalFound = 0;
		int t = omp_get_thread_num();
		for (int i = count * t; i < count * (t + 1); i++)
		{
#pragma omp critical
			intSet.unsafe_erase(i);
		}
	}

#pragma 
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Erasing:         " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms elapsed." << std::endl;
	std::cout << "Now:             \t" << intSet.size() << " items! " << std::endl;
}

void test_high_performance_set_insert(CSTXHashSet<int, thread_count> &intSet)
{
	//////////////////////////////////////////////////////////////////////////
	//CSTXHashSet
	//Insertion
	auto begin = std::chrono::high_resolution_clock::now();
#pragma omp parallel
	{
		int t = omp_get_thread_num();
		for (int i = count * t; i < count * (t + 1); i++)
			intSet.insert(i);
	}

#pragma 
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Insertion:       \t" << intSet.size() << " items! " << std::endl;
	std::cout << "Insertion:       " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms elapsed." << std::endl;

}

void test_high_performance_set_searching(CSTXHashSet<int, thread_count> &intSet)
{
	//////////////////////////////////////////////////////////////////////////
	//CSTXHashSet set
	//Searching
	auto begin = std::chrono::high_resolution_clock::now();
	std::atomic<int> totalFoundAll = 0;
#pragma omp parallel
	{
		int totalFound = 0;
		int t = omp_get_thread_num();
		for (int i = count * t; i < count * (t + 1); i++)
		{
			int targetValue = ((rand() * rand() + rand()) % count) + count * t;
			auto found = intSet.find(targetValue);
			if (found != intSet.end(targetValue))
				totalFound++;
		}
		totalFoundAll += totalFound;
	}
	std::cout << "Found:           \t" << totalFoundAll << " items." << std::endl;

#pragma 
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Searching:       " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms elapsed." << std::endl;
}

void test_high_performance_erasing(CSTXHashSet<int, thread_count> &intSet)
{
	//////////////////////////////////////////////////////////////////////////
	//CSTXHashSet set
	//Searching
	auto begin = std::chrono::high_resolution_clock::now();
	std::atomic<int> totalFoundAll = 0;
#pragma omp parallel
	{
		int totalFound = 0;
		int t = omp_get_thread_num();
		for (int i = count * t; i < count * (t + 1); i++)
		{
			intSet.erase(i);
		}
	}

#pragma 
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Erasing:         " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms elapsed." << std::endl;
	std::cout << "Now:             \t" << intSet.size() << " items! " << std::endl;
}

int main()
{
	srand((unsigned int)time(nullptr));
	omp_set_num_threads(thread_count);		//Setup threads number for OpenMP

	std::cout << "---------- STL set (std::set) ----------" << std::endl;
	std::set<int> intSetStd;
	test_std_set_insert(intSetStd);
	test_std_set_searching(intSetStd);
	test_std_set_erasing(intSetStd);
	std::cout << std::endl;

	std::cout << "---------- Concurrent set (Concurrency::concurrent_unordered_set) ----------" << std::endl;
	Concurrency::concurrent_unordered_set<int> intConcurrentSet;
	test_concurrent_set_insert(intConcurrentSet);
	test_concurrent_set_searching(intConcurrentSet);
	test_concurrent_set_erasing(intConcurrentSet);
	std::cout << std::endl;

	std::cout << "---------- High Performance set ----------" << std::endl;
	CSTXHashSet<int, thread_count> intSet;
	test_high_performance_set_insert(intSet);
	test_high_performance_set_searching(intSet);
	test_high_performance_erasing(intSet);
	std::cout << std::endl;
	
	
	return 0;
}

