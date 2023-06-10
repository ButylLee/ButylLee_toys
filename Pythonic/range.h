#pragma once

#include <type_traits>
#include <concepts>

template<std::signed_integral T = std::make_signed_t<size_t>>
class range
{
public:
	class range_iterator
	{
	public:
		constexpr range_iterator(T pos) noexcept :_pos(pos) {}
		constexpr range_iterator(T pos, T step) noexcept
			:_pos(pos), _step(step)
		{}
		constexpr T operator*() const noexcept { return _pos; }
		constexpr T operator++() noexcept { return _pos += _step; }
		constexpr T operator++(int) noexcept { return (_pos += _step) - _step; }
		constexpr bool operator==(const range_iterator& rhs) const noexcept {
			return this->_pos == rhs._pos;
		}
		constexpr bool operator!=(const range_iterator& rhs) const noexcept {
			return !(*this == rhs);
		}
	private:
		T _pos;
		T _step = T{ 1 };
	};

	constexpr range(T end) noexcept :_end(end) { check(); }
	constexpr range(T begin, T end) noexcept :_begin(begin), _end(end) { check(); }
	constexpr range(T begin, T end, T step) noexcept
		:_begin(begin), _end(end), _step(step) {
		check();
	}
	constexpr auto begin() const noexcept {
		return range_iterator(_begin, _step);
	}
	constexpr auto end() const noexcept {
		return range_iterator(_end, _step);
	}

private:
	constexpr void check() noexcept {
		if (_step > T{ 0 } && _begin > _end ||
			_step < T{ 0 } && _begin < _end || _step == T{ 0 })
			_begin = _end;
	}
	// [begin, end)
	T _begin = T{ 0 };
	T _end;
	T _step = T{ 1 };
};