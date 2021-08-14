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
	DynArrayRef(T* data_, size_t* cur_dim_size_, size_t total) noexcept
		: data(data_)
		, cur_dim_size(cur_dim_size_)
		, remain(total / *cur_dim_size_)
	{}

	auto operator[](size_t index)
	{
		return DynArrayRef<T, Dimension - 1>(data + index * remain, cur_dim_size + 1, remain);
	}

private:
	T* data;
	size_t* cur_dim_size;
	size_t remain;
};

template<typename T>
class DynArrayRef<T, 1>
{
public:
	DynArrayRef(T* data_, size_t*, size_t) noexcept
		: data(data_)
	{}

	T& operator[](size_t index) noexcept
	{
		return data[index];
	}

	const T& operator[](size_t index) const noexcept
	{
		return data[index];
	}

private:
	T* data;
};
// -------------------------------------------

namespace detail {
	template<typename Size>
	inline size_t GetTotalSizeAndFill(size_t* dim_info, Size size) noexcept
	{
		static_assert(std::is_integral_v<Size>);
		assert(size > 0);
		*dim_info = size;
		return size;
	}

	template<typename Size, typename... Sizes>
	inline size_t GetTotalSizeAndFill(size_t* dim_info, Size size, Sizes... sizes) noexcept
	{
		static_assert(std::is_integral_v<Size>);
		return GetTotalSizeAndFill(dim_info, size) * GetTotalSizeAndFill(dim_info + 1, sizes...);
	}
}

// --------------- Main class DynArray ---------------
template<typename T, size_t Dimension>
class DynArray
{
	static_assert(Dimension != 0);
public:
	template<typename... Args>
	DynArray(Args... sizes)
	{
		static_assert(sizeof...(Args) == Dimension, "The specified Dimension is not equal to sizes args");
		total_count = detail::GetTotalSizeAndFill(dim_info.sizes, sizes...);
		data = new T[total_count]{}; // zero initialized
	}
	~DynArray() noexcept
	{
		delete[] data;
	}
	DynArray(const DynArray& other)
	{
		dim_info = other.dim_info;
		total_count = other.total_count;
		data = new T[total_count];
		std::copy_n(other.data, total_count, this->data);
	}
	DynArray& operator=(const DynArray& other)
	{
		if (this == &other)
			return *this;
		DynArray temp(other);
		this->swap(temp);
		return *this;
	}
	DynArray(DynArray&& other) noexcept
	{
		this->data = other.data;
		this->dim_info = other.dim_info;
		this->total_count = other.total_count;
		other.data = nullptr;
	}
	DynArray& operator=(DynArray&& other) noexcept
	{
		if (this == &other)
			return *this;
		~DynArray();
		this->data = other.data;
		this->dim_info = other.dim_info;
		this->total_count = other.total_count;
		other.data = nullptr;
	}

	void swap(DynArray& other) noexcept
	{
		std::swap(this->data, other.data);
		std::swap(this->dim_info, other.dim_info);
		std::swap(this->total_count, other.total_count);
	}

public:
	auto ref()
	{
		return DynArrayRef<T, Dimension>(data, dim_info.sizes, total_count);
	}
	auto operator[](size_t index)
	{
		return ref()[index];
	}
	size_t size() noexcept
	{
		return dim_info.sizes[0];
	}
	size_t total_size() noexcept
	{
		return total_count;
	}

private:
	T* data;
	struct {
		size_t sizes[Dimension];
	}dim_info;
	size_t total_count;
};

#endif // DYNARRAY_HEADER_