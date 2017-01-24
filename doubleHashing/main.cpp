#include <stdlib.h>
#include <iostream>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <xstddef>
#include "double_hash_set.h"
#include "double_hash_map.h"
#include <time.h>
#include <chrono>
#include <vector>
#include <sstream>
using namespace std;

size_t losowa = 13544;

size_t fun( int num)
{
	hash<int> hasher;
	return hasher(num);
}

size_t fun2(int num)
{
	hash<int> hasher;
	return (hasher(num) % losowa) * 2 + 1;
}

size_t fun_str(string str)
{
	hash<string> hasher;
	return hasher(str);
}


//wartoœæ bie¿¹ca *c + kod asci, c nie mo¿e byæ potêg¹ 2, obliczenia prowadziæ na uint64, c = 127
size_t fun2_str(string str)
{
	hash<string> hasher;
	return (hasher(str) % losowa) * 2 + 1;
}







void test_map_int(int size)
{
	unordered_map<int, int> d;
	double_hash_map<int, int> test(&fun, &fun2);

	vector<int> insert;
	vector<int> find;


	for (size_t i = 0; i < size; i++)
	{
		int r = 0;
		int f = 0;
		insert.push_back(fun(i));
		find.push_back(fun(i));
	}


	auto start = std::chrono::steady_clock::now();
	for (size_t i = 0; i < insert.size(); i++)
	{
		std::pair<int, int> tmp(insert[i], insert[i]);
		d.insert(tmp);

	}
	auto stop = std::chrono::steady_clock::now();
	std::cout << "unordered_set inserting " << insert.size() << " elements: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << " ms" << std::endl;
	std::cout << "size: " << d.size() << std::endl;
	std::cout << "bucket_count: " << d.bucket_count() << std::endl;
	std::cout << "load_factor: " << d.load_factor() << std::endl;


	start = std::chrono::steady_clock::now();
	test.rehash(insert.size() + 1);
	for (size_t i = 0; i < insert.size(); i++)
	{
		std::pair<int, int> tmp(insert[i], insert[i]);
		test.insert(tmp);
	}
	stop = std::chrono::steady_clock::now();
	std::cout << "double_hash_table inserting " << insert.size() << " elements: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << " ms" << std::endl;
	std::cout << "size: " << test.size() << std::endl;
	std::cout << "bucket_count: " << test.bucket_count() << std::endl;
	std::cout << "load_factor: " << test.load_factor() << std::endl;



	start = std::chrono::steady_clock::now();
	for (size_t i = 0; i < insert.size(); i++)
	{
		d.find(find[i]);
	}
	stop = std::chrono::steady_clock::now();
	std::cout << "unordered_set finding " << insert.size() << " elements: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << " ms" << std::endl;


	start = std::chrono::steady_clock::now();
	for (size_t i = 0; i < find.size(); i++)
	{
		d.find(find[i]);
	}
	stop = std::chrono::steady_clock::now();
	std::cout << "double_hash_table finding " << insert.size() << " elements: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << " ms" << std::endl;

}



void test_set_int(int size)
{
	unordered_set<int> b;
	double_hash_set< int> test(&fun, &fun2);

	vector<int> insert;
	vector<int> find;

	for (size_t i = 0; i < size; i++)
	{
		int r = 0;
		int f = 0;
		insert.push_back(fun(i));
		find.push_back(fun(i));
	}


	auto start = std::chrono::steady_clock::now();
	b.rehash(insert.size()+1);
	for (size_t i = 0; i < insert.size(); i++)
	{
		b.insert(insert[i]);

	}
	auto stop =  std::chrono::steady_clock::now();
	std::cout << "unordered_set inserting " << insert.size() << " int elements: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << " ms" << std::endl;
	std::cout << "size: " << b.size() << std::endl;
	std::cout << "bucket_count: " << b.bucket_count() << std::endl;
	std::cout << "load_factor: " << b.load_factor()<< std::endl;
	

	start = std::chrono::steady_clock::now();
	test.rehash(insert.size() + 1);
	for (size_t i = 0; i < insert.size(); i++)
	{
		test.insert(insert[i]);
	}
	stop = std::chrono::steady_clock::now();
	std::cout << "double_hash_set inserting " << insert.size() << " int elements: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << " ms" << std::endl;
	std::cout << "size: " << test.size() << std::endl;
	std::cout << "bucket_count: " << test.bucket_count() << std::endl;
	std::cout << "load_factor: " << test.load_factor() << std::endl;



	start = std::chrono::steady_clock::now();
	for (size_t i = 0; i < insert.size(); i++)
	{
		b.find(find[i]);
	}
	stop = std::chrono::steady_clock::now();
	std::cout << "unordered_set finding " << insert.size() << " int elements: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << " ms" << std::endl;


	start = std::chrono::steady_clock::now();
	for (size_t i = 0; i < find.size(); i++)
	{
		test.find(find[i]);
	}
	stop = std::chrono::steady_clock::now();
	std::cout << "double_hash_set finding " << insert.size() << " int elements: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << " ms" << std::endl;
}


void test_set_string(int size)
{
	unordered_set<string> b;
	double_hash_set<string> test(&fun_str, &fun2_str);

	vector<string> insert;
	vector<string> find;
	string tmp = "aaaaaaaaaaaaaaa";
	int ind = 0;
	for (size_t i = 0; i < size; i++)
	{
		std::ostringstream ss;
		long num = fun(i);
		ss << num;
		insert.push_back(std::to_string(fun(i)));
		find.push_back(std::to_string(fun(i)));
	}


	auto start = std::chrono::steady_clock::now();
	b.rehash(insert.size() + 1);
	for (size_t i = 0; i < insert.size(); i++)
	{
		b.insert(insert[i]);

	}
	auto stop = std::chrono::steady_clock::now();
	std::cout << "unordered_set inserting " << insert.size() << " string elements: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << " ms" << std::endl;
	std::cout << "size: " << b.size() << std::endl;
	std::cout << "bucket_count: " << b.bucket_count() << std::endl;
	std::cout << "load_factor: " << b.load_factor() << std::endl;


	start = std::chrono::steady_clock::now();
	test.rehash(insert.size() + 1);
	for (size_t i = 0; i < insert.size(); i++)
	{
		test.insert(insert[i]);
	}
	stop = std::chrono::steady_clock::now();
	std::cout << "double_hash_set inserting " << insert.size() << " string elements: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << " ms" << std::endl;
	std::cout << "size: " << test.size() << std::endl;
	std::cout << "bucket_count: " << test.bucket_count() << std::endl;
	std::cout << "load_factor: " << test.load_factor() << std::endl;



	start = std::chrono::steady_clock::now();
	for (size_t i = 0; i < insert.size(); i++)
	{
		b.find(find[i]);
	}
	stop = std::chrono::steady_clock::now();
	std::cout << "unordered_set finding " << insert.size() << " string elements: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << " ms" << std::endl;


	start = std::chrono::steady_clock::now();
	for (size_t i = 0; i < find.size(); i++)
	{
		test.find(find[i]);
	}
	stop = std::chrono::steady_clock::now();
	std::cout << "double_hash_set finding " << insert.size() << " string elements: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << " ms" << std::endl;
}

void test_map_string(int size)
{
	unordered_map<string, string> b;
	double_hash_map<string, string> test(&fun_str, &fun2_str);

	vector<string> insert;
	vector<string> find;
	string tmp = "aaaaaaaaaaaaaaa";
	int ind = 0;
	for (size_t i = 0; i < size; i++)
	{
		std::ostringstream ss;
		long num = fun(i);
		ss << num;
		insert.push_back(std::to_string(fun(i)));
		find.push_back(std::to_string(fun(i)));
	}


	auto start = std::chrono::steady_clock::now();
	b.rehash(insert.size() + 1);
	for (size_t i = 0; i < insert.size(); i++)
	{
		std::pair<string, string> tmp(insert[i], insert[i]);
		b.insert(tmp);

	}
	auto stop = std::chrono::steady_clock::now();
	std::cout << "unordered_map inserting " << insert.size() << " string elements: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << " ms" << std::endl;
	std::cout << "size: " << b.size() << std::endl;
	std::cout << "bucket_count: " << b.bucket_count() << std::endl;
	std::cout << "load_factor: " << b.load_factor() << std::endl;


	start = std::chrono::steady_clock::now();
	//test.rehash(insert.size() + 1);
	for (size_t i = 0; i < insert.size(); i++)
	{
		std::pair<string, string> tmp(insert[i], insert[i]);
		test.insert(tmp);
	}
	stop = std::chrono::steady_clock::now();
	std::cout << "double_hash_map inserting " << insert.size() << " string elements: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << " ms" << std::endl;
	std::cout << "size: " << test.size() << std::endl;
	std::cout << "bucket_count: " << test.bucket_count() << std::endl;
	std::cout << "load_factor: " << test.load_factor() << std::endl;



	start = std::chrono::steady_clock::now();
	for (size_t i = 0; i < insert.size(); i++)
	{
		b.find(find[i]);
	}
	stop = std::chrono::steady_clock::now();
	std::cout << "unordered_map finding " << insert.size() << " string elements: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << " ms" << std::endl;


	start = std::chrono::steady_clock::now();
	for (size_t i = 0; i < find.size(); i++)
	{
		test.find(find[i]);
	}
	stop = std::chrono::steady_clock::now();
	std::cout << "double_hash_map finding " << insert.size() << " string elements: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << " ms" << std::endl;
}



int main()
{
	srand(time(NULL));

	//cout << fun_str("askjdfhaaksjdf") << endl;
	//cout << fun2_str("askjdafhaksjdf") << endl;
	 
	//test_set_int(1000000);
	//cout << endl << endl;
	//test_map_int(1000000);

	test_set_int (1000000);
	cout << endl << endl;
	test_set_string(1000000);

	//double_hash_set<int> test(&fun, &fun2), test2(&fun, &fun2);

	//test.insert(1);
	//test.insert(2);
	//test.insert(3);
	//test.insert(4);
	//test.insert(5);
	//test.insert(6);

	////
	////for (auto it = test.begin(); it != test.end(); ++it)
	////	std::cout << " " << *it;
	////std::cout << std::endl;

	//std::vector<int> v1(test.begin(), test.end());
	//for (size_t i = 0; i < v1.size(); i++)
	//{
	//	cout << " " << v1[i] << endl;
	//}

	//allocator<string> tmp;
	//string *a = tmp.allocate(16);
	//tmp.construct(a+1, "123123");
	//cout << a[1] << endl;
	return 0;
}