#include "range.h"
#include "enumerate.h"

#include <iostream>
#include <vector>

int main(int argc, char* argv[])
{
	for (auto&& i : range(5))
		std::cout << i << ' '; // 0 1 2 3 4
	std::cout << std::endl;

	for (auto&& i : range(-2, 2))
		std::cout << i << ' '; // -2 -1 0 1
	std::cout << std::endl;

	for (auto&& i : range(5, -5, -2))
		std::cout << i << ' '; // 5 3 1 -1 -3
	std::cout << std::endl;

	auto list = { 21,42 };
	for (auto&& [index, value] : enumerate(list))
		std::cout << index << ":" << value << ' '; // 0:21 1:42
	std::cout << std::endl;

	auto&& rlist = { 21,42 };
	for (auto&& [index, value] : enumerate(std::move(rlist)))
		std::cout << index << ":" << value << ' '; // 0:21 1:42
	std::cout << std::endl;

	std::vector<int> v{ 2,3,4 };
	for (auto&& [index, value] : enumerate(v))
		std::cout << index << ":" << value << ' ';
	std::cout << std::endl;

	const std::vector<int> cv{ 2,3,4 };
	for (auto&& [index, value] : enumerate(cv))
		std::cout << index << ":" << value << ' ';
	std::cout << std::endl;

	std::vector<int> rv{ 2,3,4 };
	for (auto&& [index, value] : enumerate(std::move(rv)))
		std::cout << index << ":" << value << ' ';
	std::cout << std::endl;
}