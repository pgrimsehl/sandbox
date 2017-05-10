#pragma once

#include "core.h"

namespace core
{
	struct in_place_t
	{
		explicit in_place_t() = default;
	};
	constexpr in_place_t in_place{};

#ifdef CORE_ONLY_STATIC_DATA_MEMBER_TEMPLATES_ALLOWED
	template <class T> struct in_place_type_t
	{
		explicit constexpr in_place_type_t()
		{
		}
	};
	template <class T> constexpr in_place_type_t<T> in_place_type = in_place_type_t<T>{};

	template <size_t I> struct in_place_index_t
	{
		explicit constexpr in_place_index_t()
		{
		};
	};
	template <size_t I> constexpr in_place_index_t<I> in_place_index = in_place_index_t<I>{};
#else
	template <class T> struct in_place_type_t
	{
		explicit in_place_type_t() = default;
	};
	template <class T> constexpr in_place_type_t<T> in_place_type{};

	template <size_t I> struct in_place_index_t
	{
		explicit in_place_index_t() = default;
	};
	template <size_t I> constexpr in_place_index_t<I> in_place_index{};
#endif
}