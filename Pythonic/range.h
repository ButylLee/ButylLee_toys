#pragma once

#include <type_traits>
#include <concepts>
#include <cassert>

template<std::integral T>
class range
{
public:
	using difference_type = std::make_signed_t<T>;
	using value_type = T;

	class range_iterator
	{
	public:
		using difference_type = range::difference_type;
		using value_type = range::value_type;
	public:
		constexpr range_iterator() = default;
		constexpr range_iterator(value_type pos, difference_type step) noexcept
			: pos_(pos), step_(step)
		{}
		constexpr value_type operator*() const noexcept { return pos_; }
		constexpr range_iterator& operator++() noexcept { pos_ += step_; return *this; }
		constexpr range_iterator operator++(int) noexcept { auto tmp = *this; ++*this; return tmp; }
		constexpr range_iterator& operator--() noexcept { pos_ -= step_; return *this; }
		constexpr range_iterator operator--(int) noexcept { auto tmp = *this; ++*this; return tmp; }
		constexpr bool operator==(const range_iterator& rhs) const noexcept
		{
			return this->pos_ == rhs.pos_;
		}
	private:
		value_type pos_ = value_type{ 0 };
		difference_type step_ = difference_type{ 1 };
	};

	template<typename V>
	constexpr range(V end) noexcept
		: end_(static_cast<value_type>(end))
	{
		check();
	}
	template<typename U, typename V>
	constexpr range(U begin, V end) noexcept
		: begin_(static_cast<value_type>(begin)), end_(static_cast<value_type>(end))
	{
		check();
	}
	template<typename U, typename V, typename W>
	constexpr range(U begin, V end, W step) noexcept
		: begin_(static_cast<value_type>(begin)), end_(static_cast<value_type>(end))
		, step_(static_cast<difference_type>(step))
	{
		check();
	}
	constexpr auto begin() const noexcept
	{
		return range_iterator(begin_, step_);
	}
	constexpr auto end() const noexcept
	{
		return range_iterator(end_, step_);
	}

private:
	constexpr void check() noexcept
	{
		if (std::is_constant_evaluated())
		{
			if (step_ > difference_type{ 0 } && begin_ > end_ ||
				step_ < difference_type{ 0 } && begin_ < end_ || step_ == difference_type{ 0 })
				begin_ = end_;
		}
		else
		{
			assert(step_ > difference_type{ 0 } && begin_ <= end_ ||
				   step_ < difference_type{ 0 } && begin_ >= end_);
		}
	}
	// [begin, end)
	value_type begin_ = value_type{ 0 };
	value_type end_;
	difference_type step_ = difference_type{ 1 };
};

template<typename V>
range(V) -> range<V>;
template<typename U, typename V>
range(U, V) -> range<V>;
template<typename U, typename V, typename W>
range(U, V, W) -> range<V>;