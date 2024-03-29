﻿#include "DynArray.h"
#include <cassert>
#include <cstdlib>
#include <utility>
#include <ranges>

int main()
{
#ifdef _MSC_VER
	assert(!std::atexit([] { assert(!_CrtDumpMemoryLeaks()); }));
#endif

	DynArray<int, 3> arr(4, 5, 6);
	assert(arr.size() == 4);
	assert(arr.size(1) == 5);
	assert(arr.size(2) == 6);
	assert(arr.total_size() == 120);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			for (int k = 0; k < 6; k++)
			{
				arr[i][j][k] = i * 5 * 6 + j * 6 + k + 1;
			}
		}
	}
	assert(arr[0][1][2] == 9);
	assert(arr[1][3][5] == 54);
	auto arr2(arr);
	assert(arr2[0][1][2] == 9);
	assert(arr2[1][3][5] == 54);
	auto arr3(std::move(arr2));
	assert(arr3[0][1][2] == 9);
	assert(arr3[1][3][5] == 54);

	DynArray<int> arr4(5);
	arr4[0] = 42;
	arr4[3] = 1;
	arr4[4] = 24;
	assert(arr4[3] == 1);
	assert(arr4.front() == 42);
	assert(arr4.ref().front() == 42);
	assert(arr4.back() == 24);
	assert(arr4.ref().back() == 24);

	DynArray<int, 3> arr5(2, 4, 6);
	assert(arr5.total_size() == 48);
	assert(arr5[1].total_size() == 24);
	assert(arr5[1][2].total_size() == 6);
	assert(arr5.size() == 2);
	assert(arr5[1].size() == 4);
	assert(arr5[1][2].size() == 6);
	assert(arr5.ref().size() == arr5.size());
	assert(arr5.ref().total_size() == arr5.total_size());

	DynArray<int, 3> arr6(2, 3, 4);
	auto arr6ref1 = arr6.ref();
	static_assert(std::is_same_v<decltype(arr6ref1), DynArrayRef<int, 3>>);
	assert(arr6ref1.size() == 2);
	assert(arr6ref1.size(1) == 3);
	assert(arr6ref1.size(2) == 4);
	auto arr6ref2 = arr6ref1[1];
	static_assert(std::is_same_v<decltype(arr6ref2), DynArrayRef<int, 2>>);
	assert(arr6ref2.size() == 3);
	assert(arr6ref2.size(1) == 4);
	auto arr6ref3 = arr6ref2[2];
	static_assert(std::is_same_v<decltype(arr6ref3), DynArrayRef<int, 1>>);
	assert(arr6ref3.size() == 4);
	auto arr6val = arr6ref3[3];
	static_assert(std::is_same_v<decltype(arr6val), int>);

	DynArray<int, 2> arr7(2, 3);
	arr7[1][2] = 42;
	const DynArray<int, 2>& arr7ref = arr7;
	assert(arr7ref[1][2] == 42);

	DynArray<int, 3> arr8(2, 3, 4);
	int num = 0;
	for (auto& row : arr8)
	{
		for (auto& column : row)
		{
			for (auto& item : column)
			{
				item = num++;
			}
		}
	}

	{
		int count = 0;
		num = 0;
		for (const auto& i : arr8.iter_all())
		{
			assert(i == num++);
			count++;
		}
		assert(count == arr8.total_size());
	}
	{
		DynArray<int> arr(10);
		for (size_t i = 0; i < 10; i++)
			arr[i] = i;
		for (size_t i = 9; auto num : arr | std::views::reverse)
			assert(num == i--);
		for (size_t i = 9; auto num:arr.iter_all() | std::views::reverse)
			assert(num == i--);
	}
}
