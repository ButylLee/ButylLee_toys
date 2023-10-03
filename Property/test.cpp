#include "Property.h"
#include <cassert>

void f1(short value)
{
	(void)value;
}

struct Num1
{
	short i = 42;
	operator short() const noexcept
	{
		return i;
	}
};

struct Num2
{
	short i;
	Num2(short v) :i(v) {}
	operator short() const noexcept
	{
		return i;
	}
};

class A
{
	UsingProperty(A);
public:
	Property<int, None> i1{ 1 };
	Property<int, Get> i2{ 2 };
	Property<int, Set> i3{ 3 };
	Property<int, GetSet> i4{ 4 };
	void foo()
	{
		i1.value = 10;
		i2.value = 20;
		i3.value = 30;
		i4.value = 40;
		assert(i1.value == 10);
		assert(i2.value == 20);
		assert(i3.value == 30);
		assert(i4.value == 40);
	}

	Property<Num1, GetSet> num1;
	Property<Num2, GetSet> num2{ 42 };
	Property<Num2, GetSet> num3;
	A() : num3(42) {}
};

int main()
{
	A a;
	a.foo();

	//a.i1 = 42;
	//f1(a.i1);

	//a.i2 = 42;
	f1(a.i2);
	f1(a.i2.get());

	a.i3 = 42;
	a.i3.set(24);
	//f1(a.i3);

	a.i4 = 42;
	f1(a.i4);

	a.num1 = 24;
	f1(a.num1.get()); // use get() to cope with 2-step implicit cast

	assert(a.num2.get() == 42);
	assert(a.num3.get() == 42);
}