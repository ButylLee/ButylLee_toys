#include "DynArray.h"
#include <cassert>
#include <cstdlib>

int main()
{
#ifdef _MSC_VER
	assert(!std::atexit([] { assert(!_CrtDumpMemoryLeaks()); }));
#endif

	DynArray<int, 3> arr(4, 5, 6);
	assert(arr.size() == 4);
	assert(arr.total_size() == 120);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			for (int k = 0; k < 6; k++)
			{
				arr[i][j][k] = (i + 1) * (j + 1) * (k + 1);
			}
		}
	}
	assert(arr[0][1][2] == 6);
	assert(arr[1][3][5] == 48);
	auto arr2(arr);
	assert(arr2[0][1][2] == 6);
	assert(arr2[1][3][5] == 48);
	auto arr3(std::move(arr2));
	assert(arr3[0][1][2] == 6);
	assert(arr3[1][3][5] == 48);

	DynArray<int, 1> arr4(5);
	arr4[3] = 1;
	assert(arr4[3] == 1);
}