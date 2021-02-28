#include <utility>
#include <memory>
#include <type_traits>

namespace NSVarTypeDict{
	struct NullParameter{};

	///////////////////////////////////////////////////////
	template<size_t N, template<typename...> class Values>
	struct Create_
	{
		template<typename... TTypes>
		using NextType = Values<NullParameter, TTypes...>;

		using type = typename Create_<N - 1, NextType>::type;
	};

	template<template<typename...> class Values>
	struct Create_<1, Values>
	{
		using type = Values<NullParameter>;
	};

	///////////////////////////////////////////////////////
	template<typename TFindTag, size_t N, typename TCurTag, typename... TTags>
	struct Tag2ID_ // The CurTag doesn't match and proceed next Tag
	{
		constexpr static size_t value = Tag2ID_<TFindTag, N + 1, TTags...>::value;
	};
	template<typename TFindTag, size_t N, typename... TTags>
	struct Tag2ID_<TFindTag, N, TFindTag, TTags...> // Find the Tag
	{
		constexpr static size_t value = N;
	};

	template<typename TFindTag, typename... TTags>
	constexpr size_t Tag2ID = Tag2ID_<TFindTag, 0, TTags...>::value;

	///////////////////////////////////////////////////////
	template<typename NewType, size_t N, size_t M, typename TCont, typename... TTypes>
	struct NewTupleType_;

	// when N != M
	template<typename NewType, size_t N, size_t M,
		template<typename...> class TCont,
		typename... TModifiedTypes,
		typename TCur,
		typename... TRemainTypes>
	struct NewTupleType_<NewType, N, M, TCont<TModifiedTypes...>, TCur, TRemainTypes...>
	{
		using type = typename NewTupleType_<NewType, N + 1, M, TCont<TModifiedTypes..., TCur>, TRemainTypes...>::type;
	};

	// when N == M
	template<typename NewType, size_t N,
		template<typename...> class TCont,
		typename... TModifiedTypes,
		typename TCur,
		typename... TRemainTypes>
	struct NewTupleType_<NewType, N, N, TCont<TModifiedTypes...>, TCur, TRemainTypes...>
	{
		using type = TCont<TModifiedTypes..., NewType, TRemainTypes...>;
	};

	template<typename NewType, size_t Pos, typename TCont, typename... TTypes>
	using NewTupleType = typename NewTupleType_<NewType, 0, Pos, TCont, TTypes...>::type;

	///////////////////////////////////////////////////////
	template<size_t N, size_t M, typename TCur, typename... TTypes>
	struct Pos2Type_
	{
		using type = typename Pos2Type_<N + 1, M, TTypes...>::type;
	};
	template<size_t N, typename TCur, typename... TTypes>
	struct Pos2Type_<N, N, TCur, TTypes...>
	{
		using type = TCur;
	};

	template<size_t Pos, typename... TTypes>
	using Pos2Type = typename Pos2Type_<0, Pos, TTypes...>::type;
} // namespace NSVarTypeDict


template<typename... TParameters>
struct VarTypeDict
{
	template<typename... TTypes>
	struct Values{
		Values() = default;
		Values(std::shared_ptr<void>(&& input)[sizeof...(TTypes)])
		{
			for (size_t i = 0; i < sizeof...(TTypes); i++)
			{
				m_tuple[i] = std::move(input[i]);
			}
		}

	public:
		template<typename TTag, typename TVal>
		auto Set(TVal&& val)&&
		{
			using namespace NSVarTypeDict;
			constexpr static size_t TagPos = Tag2ID<TTag, TParameters...>;

			using rawTVal = std::decay_t<TVal>;
			rawTVal* tmp = new rawTVal(std::forward<TVal>(val));
			m_tuple[TagPos] = std::shared_ptr<void>(tmp,
													[](void* ptr) noexcept {
														rawTVal* nptr = static_cast<rawTVal*>(ptr);
														delete nptr;
													});

			using NewTypes = NewTupleType<rawTVal, TagPos, Values<>, TTypes...>;
			return NewTypes(std::move(m_tuple));
		}

		template<typename TTag>
		const auto& Get() const
		{
			using namespace NSVarTypeDict;
			constexpr static size_t TagPos = Tag2ID<TTag, TParameters...>;

			using rawType = Pos2Type<TagPos, TTypes...>;
			return *static_cast<rawType*>(m_tuple[TagPos].get());
		}

		template<typename TTag>
		using ValueType = NSVarTypeDict::Pos2Type<NSVarTypeDict::Tag2ID<TTag, TParameters...>, TTypes...>;

	private:
		std::shared_ptr<void> m_tuple[sizeof...(TTypes)];
	};

	static auto Create()
	{
		using namespace NSVarTypeDict;
		using type = typename Create_<sizeof...(TParameters), Values>::type;

		return type();
	}
};
