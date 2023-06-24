#pragma once
#ifndef DYNARRAY_HEADER_
#define DYNARRAY_HEADER_

#include <type_traits>
#include <algorithm>
#include <utility>
#include <cassert>
#include <cstddef>

// ------------------- proxy class -------------------
template<typename T, size_t Dimension>
class DynArrayRef
{
public:
	DynArrayRef(T* data, const size_t* sizes, const size_t* remains) noexcept
		: arr_data(data), arr_size(sizes), arr_remain(remains)
	{}

	DynArrayRef<T, Dimension - 1> operator[](size_t index) const noexcept
	{
		assert(index < arr_size[0]);
		return DynArrayRef<T, Dimension - 1>(arr_data + arr_remain[0] * index, arr_size + 1, arr_remain + 1);
	}
	DynArrayRef<T, Dimension - 1> front() const noexcept
	{
		return (*this)[0];
	}
	DynArrayRef<T, Dimension - 1> back() const noexcept
	{
		return (*this)[arr_size[0] - 1];
	}
	size_t size(size_t dimension = 0) const noexcept
	{
		assert(dimension < Dimension);
		return arr_size[dimension];
	}
	size_t total_size() const noexcept
	{
		return arr_remain[0] * arr_size[0];
	}
	T* data() const noexcept
	{
		return arr_data;
	}

private:
	T* arr_data;
	const size_t* arr_size;
	const size_t* arr_remain;
};

template<typename T>
class DynArrayRef<T, 1>
{
public:
	DynArrayRef(T* data, const size_t* sizes, const size_t*) noexcept
		: arr_data(data), arr_size(sizes)
	{}

	T& operator[](size_t index) const noexcept
	{
		assert(index < arr_size[0]);
		return arr_data[index];
	}
	T& front() const noexcept
	{
		return (*this)[0];
	}
	T& back() const noexcept
	{
		return (*this)[arr_size[0] - 1];
	}
	size_t size(size_t dimension = 0) const noexcept
	{
		assert(dimension == 0);
		return arr_size[0];
	}
	size_t total_size() const noexcept
	{
		return size();
	}
	T* data() const noexcept
	{
		return arr_data;
	}

private:
	T* arr_data;
	const size_t* arr_size;
};

// ----------------- iterator class ------------------
template<typename T, size_t Dimension>
class DynArrayIterator
{
public:
	DynArrayIterator(T* data, const size_t* size, const size_t* remain) noexcept
		: arr_data(data), arr_size(size), arr_remain(remain)
	{}

	const DynArrayIterator& operator*() const noexcept
	{
		return *this;
	}
	DynArrayIterator& operator++() noexcept
	{
		arr_data += arr_remain[0];
		return *this;
	}
	DynArrayIterator operator++(int) noexcept
	{
		DynArrayIterator tmp = *this;
		++*this;
		return tmp;
	}
	DynArrayIterator& operator--() noexcept
	{
		arr_data -= arr_remain[0];
		return *this;
	}
	DynArrayIterator operator--(int) noexcept
	{
		DynArrayIterator tmp = *this;
		--*this;
		return tmp;
	}
	DynArrayIterator& operator+=(size_t offset) noexcept
	{
		arr_data += arr_remain[0] * offset;
		return *this;
	}
	DynArrayIterator& operator-=(size_t offset) noexcept
	{
		arr_data -= arr_remain[0] * offset;
		return *this;
	}
	friend DynArrayIterator operator+(DynArrayIterator lhs, size_t offset) noexcept
	{
		lhs += offset;
		return lhs;
	}
	friend DynArrayIterator operator+(size_t offset, DynArrayIterator rhs) noexcept
	{
		rhs += offset;
		return rhs;
	}
	friend DynArrayIterator operator-(DynArrayIterator lhs, size_t offset) noexcept
	{
		lhs -= offset;
		return lhs;
	}
	friend size_t operator-(const DynArrayIterator& lhs, const DynArrayIterator& rhs) noexcept
	{
		assert(lhs.arr_remain[0] == rhs.arr_remain[0]);
		return (lhs.arr_data - rhs.arr_data) / lhs.arr_remain[0];
	}

	bool operator==(const DynArrayIterator& rhs) const noexcept
	{
		return this->arr_data == rhs.arr_data;
	}
	bool operator!=(const DynArrayIterator& rhs) const noexcept
	{
		return !(*this == rhs);
	}
	bool operator<(const DynArrayIterator& rhs) const noexcept
	{
		return this->arr_data < rhs.arr_data;
	}
	bool operator>(const DynArrayIterator& rhs) const noexcept
	{
		return rhs < *this;
	}
	bool operator<=(const DynArrayIterator& rhs) const noexcept
	{
		return !(rhs < *this);
	}
	bool operator>=(const DynArrayIterator& rhs) const noexcept
	{
		return !(*this < rhs);
	}

	DynArrayIterator<T, Dimension - 1> begin() const noexcept
	{
		return DynArrayIterator<T, Dimension - 1>(arr_data, arr_size + 1, arr_remain + 1);
	}
	DynArrayIterator<T, Dimension - 1> end() const noexcept
	{
		return DynArrayIterator<T, Dimension - 1>(arr_data + arr_remain[0], arr_size + 1, arr_remain + 1);
	}

private:
	T* arr_data;
	const size_t* arr_size;
	const size_t* arr_remain;
};

template<typename T>
class DynArrayIterator<T, 1>
{
public:
	DynArrayIterator(T* data, const size_t*, const size_t*) noexcept
		: arr_data(data)
	{}

	T& operator*() const noexcept
	{
		return *arr_data;
	}
	DynArrayIterator& operator++() noexcept
	{
		++arr_data;
		return *this;
	}
	DynArrayIterator operator++(int) noexcept
	{
		DynArrayIterator tmp = *this;
		++*this;
		return tmp;
	}
	DynArrayIterator& operator--() noexcept
	{
		--arr_data;
		return *this;
	}
	DynArrayIterator operator--(int) noexcept
	{
		DynArrayIterator tmp = *this;
		--*this;
		return tmp;
	}
	DynArrayIterator& operator+=(size_t offset) noexcept
	{
		arr_data += offset;
		return *this;
	}
	DynArrayIterator& operator-=(size_t offset) noexcept
	{
		arr_data -= offset;
		return *this;
	}
	friend DynArrayIterator operator+(DynArrayIterator lhs, size_t offset) noexcept
	{
		lhs += offset;
		return lhs;
	}
	friend DynArrayIterator operator+(size_t offset, DynArrayIterator rhs) noexcept
	{
		rhs += offset;
		return rhs;
	}
	friend DynArrayIterator operator-(DynArrayIterator lhs, size_t offset) noexcept
	{
		lhs -= offset;
		return lhs;
	}
	friend size_t operator-(const DynArrayIterator& lhs, const DynArrayIterator& rhs) noexcept
	{
		return lhs.arr_data - rhs.arr_data;
	}
	T& operator[](size_t index) const noexcept
	{
		return arr_data[index];
	}
	bool operator==(const DynArrayIterator& rhs) const noexcept
	{
		return this->arr_data == rhs.arr_data;
	}
	bool operator!=(const DynArrayIterator& rhs) const noexcept
	{
		return !(*this == rhs);
	}
	bool operator<(const DynArrayIterator& rhs) const noexcept
	{
		return this->arr_data < rhs.arr_data;
	}
	bool operator>(const DynArrayIterator& rhs) const noexcept
	{
		return rhs < *this;
	}
	bool operator<=(const DynArrayIterator& rhs) const noexcept
	{
		return !(rhs < *this);
	}
	bool operator>=(const DynArrayIterator& rhs) const noexcept
	{
		return !(*this < rhs);
	}

private:
	T* arr_data;
};

template<typename T>
class DynArrayFlatIterator
{
public:
	DynArrayFlatIterator(T* begin, T* end) noexcept
		: arr_begin(begin), arr_end(end)
	{}

	T* begin() const noexcept
	{
		return arr_begin;
	}

	T* end() const noexcept
	{
		return arr_end;
	}

private:
	T* arr_begin;
	T* arr_end;
};

// ---------------- detail functions -----------------
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

// --------------- main class DynArray ---------------
template<typename T, size_t Dimension = 1>
class DynArray
{
	static_assert(Dimension != 0, "The dimension of DynArray should not be zero.");
public:
	template<typename... Args>
	explicit DynArray(Args... sizes)
		: total_count{ ::detail::GetTotalSizeAndFill(dim_info.sizes, sizes...) }
		, arr_data{ ::detail::NullptrAndFillRemains(Dimension, dim_info.remains, dim_info.sizes) }
	{
		static_assert(sizeof...(Args) == Dimension,
					  "The specified Dimension is not equal to the count of sizes args.");
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
#ifndef NDEBUG
		arr_data = nullptr;
		total_count = 0;
		dim_info = {};
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

	DynArray(DynArray&& other) noexcept
	{
		this->dim_info = other.dim_info;
		this->total_count = other.total_count;
		this->arr_data = other.arr_data;
		other.arr_data = nullptr;
#ifndef NDEBUG
		other.~DynArray();
#endif
	}

	DynArray& operator=(DynArray other) & noexcept
	{
		this->swap(other);
		return *this;
	}

	void swap(DynArray& other) noexcept
	{
		using std::swap;
		swap(this->arr_data, other.arr_data);
		swap(this->dim_info, other.dim_info);
		swap(this->total_count, other.total_count);
	}

public:
	DynArrayRef<T, Dimension> ref() noexcept
	{
		return DynArrayRef<T, Dimension>(arr_data, dim_info.sizes, dim_info.remains);
	}
	DynArrayRef<const T, Dimension> ref() const noexcept
	{
		return DynArrayRef<const T, Dimension>(arr_data, dim_info.sizes, dim_info.remains);
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
	size_t size(size_t dimension = 0) const noexcept
	{
		assert(dimension < Dimension);
		return dim_info.sizes[dimension];
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
	DynArrayIterator<T, Dimension> begin() noexcept
	{
		return DynArrayIterator<T, Dimension>(arr_data, dim_info.sizes, dim_info.remains);
	}
	DynArrayIterator<const T, Dimension> begin() const noexcept
	{
		return DynArrayIterator<const T, Dimension>(arr_data, dim_info.sizes, dim_info.remains);
	}
	DynArrayIterator<T, Dimension> end() noexcept
	{
		return DynArrayIterator<T, Dimension>(arr_data + dim_info.sizes[0] * dim_info.remains[0],
											  dim_info.sizes, dim_info.remains);
	}
	DynArrayIterator<const T, Dimension> end() const noexcept
	{
		return DynArrayIterator<const T, Dimension>(arr_data + dim_info.sizes[0] * dim_info.remains[0],
													dim_info.sizes, dim_info.remains);
	}
	DynArrayFlatIterator<T> iter_all() noexcept
	{
		return DynArrayFlatIterator<T>(arr_data, arr_data + total_count);
	}
	DynArrayFlatIterator<const T> iter_all() const noexcept
	{
		return DynArrayFlatIterator<const T>(arr_data, arr_data + total_count);
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