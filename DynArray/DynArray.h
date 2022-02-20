#pragma once
#ifndef DYNARRAY_HEADER_
#define DYNARRAY_HEADER_

#include <type_traits>
#include <algorithm>
#include <utility>
#include <cassert>

// --------------- proxy class ---------------
template<typename T, size_t Dimension>
class DynArrayRef
{
public:
	DynArrayRef(T* data_, size_t* sizes, size_t* remains) noexcept
		: data(data_), my_size(sizes), my_remain(remains)
	{}

	auto operator[](size_t index) const noexcept
	{
		assert(index < *my_size);
		return DynArrayRef<T, Dimension - 1>(data + *my_remain * index, my_size + 1, my_remain + 1);
	}
	size_t size() const noexcept
	{
		return *my_size;
	}
	size_t total_size() const noexcept
	{
		return *my_remain * *my_size;
	}

private:
	T* const data;
	size_t* const my_size;
	size_t* const my_remain;
};

template<typename T>
class DynArrayRef<T, 1>
{
public:
	DynArrayRef(T* data_, size_t* sizes, size_t*) noexcept
		: data(data_), my_size(sizes)
	{}

	T& operator[](size_t index) noexcept
	{
		return const_cast<T&>(static_cast<const DynArrayRef&>(*this)[index]);
	}
	const T& operator[](size_t index) const noexcept
	{
		assert(index < *my_size);
		return data[index];
	}
	size_t size() const noexcept
	{
		return *my_size;
	}
	size_t total_size() const noexcept
	{
		return size();
	}

private:
	T* const data;
	size_t* const my_size;
};
// -------------------------------------------

namespace detail {
	template<typename TSize>
	inline size_t GetTotalSizeAndFill(size_t* dim_size, TSize size) noexcept
	{
		assert(size > 0);
		*dim_size = static_cast<size_t>(size);
		return static_cast<size_t>(size);
	}

	template<typename TSize, typename... TSizes>
	inline size_t GetTotalSizeAndFill(size_t* dim_size, TSize size, TSizes... sizes) noexcept
	{
		return GetTotalSizeAndFill(dim_size, size) * GetTotalSizeAndFill(dim_size + 1, sizes...);
	}

	inline auto NullptrAndFillRemains(size_t dimension, size_t* remains, const size_t* sizes) noexcept
	{
		// fill the remains beginning from back
		remains += dimension - 1;
		sizes += dimension - 1;
		*remains-- = 1;
		while (--dimension)
		{
			remains[0] = sizes[0] * remains[1];
			remains--;
			sizes--;
		}
		return nullptr;
	}
}

// --------------- Main class DynArray ---------------
template<typename T, size_t Dimension = 1>
class DynArray
{
	static_assert(Dimension != 0, "The dimension of DynArray should not be zero.");
public:
	template<typename... Args>
	explicit DynArray(Args... sizes)
		: total_count{ detail::GetTotalSizeAndFill(dim_info.sizes, sizes...) }
		, data{ detail::NullptrAndFillRemains(Dimension, dim_info.remains, dim_info.sizes) }
	{
		static_assert(sizeof...(Args) == Dimension, "The specified Dimension is not equal to sizes args.");
		static_assert((std::is_convertible_v<Args, size_t> && ...),
					  "The size types of dimensions must be convertible to size_t.");
#ifndef NDEBUG
		data = new T[total_count]{}; // zero initialized
#else
		data = new T[total_count];
#endif
	}

	~DynArray() noexcept
	{
		delete[] data;
		data = nullptr;
#ifndef NDEBUG
		total_count = 0;
		std::fill(std::begin(dim_info.sizes), std::end(dim_info.sizes), 0);
		std::fill(std::begin(dim_info.remains), std::end(dim_info.remains), 0);
#endif
	}

	DynArray(const DynArray& other)
	{
		this->dim_info = other.dim_info;
		this->total_count = other.total_count;
#ifndef NDEBUG
		this->data = new T[total_count]{};
#else
		this->data = new T[total_count];
#endif
		std::copy_n(other.data, total_count, this->data);
	}

	DynArray& operator=(const DynArray& other)&
	{
		if (this == &other)
			return *this;
		DynArray temp(other);
		// not checking each dimensions' size equal
		// assign two different DynArrays is not recommended
		this->swap(temp);
		return *this;
	}

	DynArray(DynArray&& other) noexcept
	{
		this->data = other.data;
		this->dim_info = other.dim_info;
		this->total_count = other.total_count;
		other.data = nullptr;
#ifndef NDEBUG
		other.~DynArray();
#endif
	}

	DynArray& operator=(DynArray&& other) & noexcept
	{
		if (this == &other)
			return *this;
		DynArray temp(other);
		// not checking each dimensions' size equal
		// assign two different DynArrays is not recommended
		this->swap(temp);
		return *this;
	}

	void swap(DynArray& other) noexcept
	{
		std::swap(this->data, other.data);
		std::swap(this->dim_info, other.dim_info);
		std::swap(this->total_count, other.total_count);
	}

public:
	auto ref() noexcept
	{
		return DynArrayRef<T, Dimension>(data, dim_info.sizes, dim_info.remains);
	}
	decltype(auto) operator[](size_t index) noexcept
	{
		return ref()[index];
	}
	size_t size() const noexcept
	{
		return dim_info.sizes[0];
	}
	size_t total_size() const noexcept
	{
		return total_count;
	}

private:
	struct { // for assigning conveniently
		size_t sizes[Dimension];
		size_t remains[Dimension];
	}dim_info;
	size_t total_count;
	T* data;
};

#endif // DYNARRAY_HEADER_