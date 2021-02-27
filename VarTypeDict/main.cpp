#include <iostream>
#include "VarTypeDict.h"
using namespace std;

// 声明一个有三个键值对的异类词典
using MyDict = VarTypeDict<struct A, struct B, struct C>;

template<typename In>
void foo(const In& in)
{
	cout << typeid(In).name() << endl;

	auto a = in.Get<A>();
	auto b = in.Get<B>();
	auto c = in.Get<C>();

	cout << (a ? b : c) << endl;
}

int main()
{
	foo(MyDict::Create()
		.Set<A>(false)
		.Set<C>(2.0F)
		.Set<B>('1'));

	return 0;
}
