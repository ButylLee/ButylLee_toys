#pragma once

#include <utility>
#include <initializer_list>
#include <iterator>

template<typename Container>
class enumerate
{
public:
	class enumerate_iterator
	{
	public:
		using Iter = decltype(std::begin(std::declval<Container>()));

		template<typename V>
		struct item_pair
		{
			template<typename T>
			constexpr item_pair(size_t index, T&& value) noexcept
				:_index(index), _value(std::forward<T>(value))
			{}
			size_t _index;
			V _value;
		};
		template<typename V>
		item_pair(size_t, V&&)->item_pair<V>;

	public:
		constexpr enumerate_iterator(size_t index, Iter iter) noexcept
			:_index(index), _iter(iter)
		{}
		constexpr auto operator*() const noexcept {
			return item_pair(_index, *_iter);
		}
		constexpr enumerate_iterator& operator++() noexcept {
			++_index;
			++_iter;
			return *this;
		}
		constexpr enumerate_iterator operator++(int) noexcept {
			enumerate_iterator tmp = *this;
			this->operator++();
			return tmp;
		}
		constexpr bool operator==(const enumerate_iterator& rhs) const noexcept {
			return this->_iter == rhs._iter;
		}
		constexpr bool operator!=(const enumerate_iterator& rhs) const noexcept {
			return !(*this == rhs);
		}

	private:
		size_t _index = 0;
		Iter _iter;
	};

public:
	template<typename T>
	constexpr enumerate(T&& container) noexcept
		:_container(std::forward<T>(container))
	{}
	template<typename T>
	constexpr enumerate(std::initializer_list<T> container) noexcept
		:_container(container)
	{}
	constexpr auto begin() noexcept {
		return enumerate_iterator(0, _container.begin());
	}
	constexpr auto begin() const noexcept {
		return enumerate_iterator(0, _container.begin());
	}
	constexpr auto end() noexcept {
		return enumerate_iterator(_container.size(), _container.end());
	}
	constexpr auto end() const noexcept {
		return enumerate_iterator(_container.size(), _container.end());
	}

private:
	Container _container;
};

template<typename T>
enumerate(std::initializer_list<T>)->enumerate<std::initializer_list<T>>;
template<typename T>
enumerate(T&&)->enumerate<T>;