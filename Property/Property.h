#pragma once
#ifndef PROPERTY_HEADER_
#define PROPERTY_HEADER_

#include <type_traits>
#include <concepts>
#include <utility>

enum struct AccessLevel
{
	None, Get, Set, GetSet
};

namespace detail
{
	template<AccessLevel Level>
	concept Getable = Level == AccessLevel::Get || Level == AccessLevel::GetSet;
	template<AccessLevel Level>
	concept Setable = Level == AccessLevel::Set || Level == AccessLevel::GetSet;
	template<typename T, typename Arg>
	constexpr bool CanNothrowInit =
		std::is_nothrow_constructible_v<T, Arg>;
	template<typename T, typename Arg>
	constexpr bool CanNothrowSet =
		std::is_nothrow_constructible_v<T, Arg>&&
		std::is_nothrow_assignable_v<T&, T>;
}

template<typename Owner, typename T, AccessLevel Level = AccessLevel::None>
class Property
{
	friend typename Owner;
public:
	using Type = std::decay_t<T>;
	using GetType = const Type&;

public:
	Property() requires std::default_initializable<Type> = default;

	template<typename V = Type>
	explicit constexpr Property(V&& value) noexcept(detail::CanNothrowInit<Type, V>)
		: value(std::forward<V>(value))
	{}

public:
	constexpr GetType get() const noexcept requires detail::Getable<Level>
	{
		return value;
	}

	constexpr operator GetType() const noexcept requires detail::Getable<Level>
	{
		return value;
	}

	template<typename V = Type>
	constexpr void set(V&& other) noexcept(detail::CanNothrowSet<Type, V>)
		requires detail::Setable<Level>
	{
		value = Type(std::forward<V>(other));
	}

	template<typename V = Type>
	constexpr Property& operator=(V&& other) noexcept(detail::CanNothrowSet<Type, V>)
		requires detail::Setable<Level>
	{
		value = Type(std::forward<V>(other));
		return *this;
	}

private:
	Type value;
};

#define UsingProperty(OwnerType) \
	using enum AccessLevel; \
	template<typename T, AccessLevel Level> \
	using Property = Property<OwnerType, T, Level>

#endif // PROPERTY_HEADER_