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
	DynArrayRef(const T* data, const size_t* sizes, const size_t* remains) noexcept
		: arr_data(data), arr_size(sizes), arr_remain(remains)
	{}

	DynArrayRef<T, Dimension - 1> operator[](size_t index) noexcept
	{
		return const_cast<DynArrayRef<T, Dimension - 1>&&>(
			static_cast<const DynArrayRef&>(*this)[index]);
	}
	const DynArrayRef<T, Dimension - 1> operator[](size_t index) const noexcept
	{
		assert(index < *arr_size);
		return DynArrayRef<T, Dimension - 1>(arr_data + *arr_remain * index, arr_size + 1, arr_remain + 1);
	}
	DynArrayRef<T, Dimension - 1> front() noexcept
	{
		return (*this)[0];
	}
	const DynArrayRef<T, Dimension - 1> front() const noexcept
	{
		return (*this)[0];
	}
	DynArrayRef<T, Dimension - 1> back() noexcept
	{
		return (*this)[*arr_size - 1];
	}
	const DynArrayRef<T, Dimension - 1> back() const noexcept
	{
		return (*this)[*arr_size - 1];
	}
	size_t size() const noexcept
	{
		return *arr_size;
	}
	size_t total_size() const noexcept
	{
		return *arr_remain * *arr_size;
	}
	T* data() noexcept
	{
		return const_cast<T*>(arr_data);
	}
	const T* data() const noexcept
	{
		return arr_data;
	}

private:
	const T* arr_data;
	const size_t* arr_size;
	const size_t* arr_remain;
};

template<typename T>
class DynArrayRef<T, 1>
{
public:
	DynArrayRef(const T* data, const size_t* sizes, const size_t*) noexcept
		: arr_data(data), arr_size(sizes)
	{}

	T& operator[](size_t index) noexcept
	{
		return const_cast<T&>(static_cast<const DynArrayRef&>(*this)[index]);
	}
	const T& operator[](size_t index) const noexcept
	{
		assert(index < *arr_size);
		return arr_data[index];
	}
	T& front() noexcept
	{
		return (*this)[0];
	}
	const T& front() const noexcept
	{
		return (*this)[0];
	}
	T& back() noexcept
	{
		return (*this)[*arr_size - 1];
	}
	const T& back() const noexcept
	{
		return (*this)[*arr_size - 1];
	}
	size_t size() const noexcept
	{
		return *arr_size;
	}
	size_t total_size() const noexcept
	{
		return size();
	}
	T* data() noexcept
	{
		return const_cast<T*>(arr_data);
	}
	const T* data() const noexcept
	{
		return arr_data;
	}

private:
	const T* arr_data;
	const size_t* arr_size;
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
		, arr_data{ detail::NullptrAndFillRemains(Dimension, dim_info.remains, dim_info.sizes) }
	{
		static_assert(sizeof...(Args) == Dimension, "The specified Dimension is not equal to sizes args.");
		static_assert((std::is_convertible_v<Args, size_t> && ...),
					  "The size types of dimensions must be convertible to size_t.");
#ifndef NDEBUG
		arr_data = new T[total_count]{}; // zero initialized
#else
		arr_data = new T[total_count];
#endif
	}

	~DynArray() noexcept
	{
		delete[] arr_data;
		arr_data = nullptr;
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
		this->arr_data = new T[total_count]{};
#else
		this->arr_data = new T[total_count];
#endif
		std::copy_n(other.arr_data, total_count, this->arr_data);
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
		this->arr_data = other.arr_data;
		this->dim_info = other.dim_info;
		this->total_count = other.total_count;
		other.arr_data = nullptr;
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
		std::swap(this->arr_data, other.arr_data);
		std::swap(this->dim_info, other.dim_info);
		std::swap(this->total_count, other.total_count);
	}

public:
	DynArrayRef<T, Dimension> ref() noexcept
	{
		return DynArrayRef<T, Dimension>(arr_data, dim_info.sizes, dim_info.remains);
	}
	const DynArrayRef<T, Dimension> ref() const noexcept
	{
		return DynArrayRef<T, Dimension>(arr_data, dim_info.sizes, dim_info.remains);
	}
	decltype(auto) operator[](size_t index) noexcept
	{
		return ref()[index];
	}
	decltype(auto) operator[](size_t index) const noexcept
	{
		return ref()[index];
	}
	decltype(auto) front() noexcept
	{
		return ref()[0];
	}
	decltype(auto) front() const noexcept
	{
		return ref()[0];
	}
	decltype(auto) back() noexcept
	{
		return ref()[size() - 1];
	}
	decltype(auto) back() const noexcept
	{
		return ref()[size() - 1];
	}
	size_t size() const noexcept
	{
		return dim_info.sizes[0];
	}
	size_t total_size() const noexcept
	{
		return total_count;
	}
	T* data() noexcept
	{
		return arr_data;
	}
	const T* data() const noexcept
	{
		return arr_data;
	}

private:
	struct { // for assigning conveniently
		size_t sizes[Dimension];
		size_t remains[Dimension];
	}dim_info;
	size_t total_count;
	T* arr_data;
};

#endif // DYNARRAY_HEADER_