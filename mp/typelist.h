#pragma once

namespace mp
{
	namespace tl
	{
		// --------------------------------------------------------------------------
		// mp::tl::TypeList
		// --------------------------------------------------------------------------
		template <class... Types> struct TypeList
		{
		};

		// --------------------------------------------------------------------------
		// mp::tl::index_of_type
		// --------------------------------------------------------------------------
		// get index of a type in a type list
		template <class L, class T> struct index_of_type_impl;
		// class V is head of list L, so index is 0
		template <template <class...> class L, class... T, class V> struct index_of_type_impl<L<V, T...>, V>
		{
			using type = std::integral_constant<size_t, 0>;
		};
		// class V is not head of list L, so increment index and call recursively
		template <template <class...> class L, class T1, class... T, class V> struct index_of_type_impl<L<T1, T...>, V>
		{
			using type = std::integral_constant<size_t, 1 + index_of_type_impl<L<T...>, V>::type::value>;
		};
		// convenience template to access index type
		template <class L, class V> using index_of_type = typename index_of_type_impl<L, V>::type;

		// --------------------------------------------------------------------------
		// mp::tl::type_at
		// --------------------------------------------------------------------------
		// get type T at index i in type list L
		template <class L, size_t> struct type_at;
		// index is 0, return head of list
		template <template <class...> class L, class... Ts, class T> struct type_at<L<T, Ts...>, 0>
		{
			using type = T;
		};
		// index is > 0, so call recursively
		template <template <class...> class L, class T, class... Ts, size_t I> struct type_at<L<T, Ts...>, I>
		{
			static_assert( 0 < sizeof...( Ts ), "index out of bounds" );
			using type = typename type_at<L<Ts...>, I - 1>::type;
		};
		// convenience template to access index type
		template <class L, size_t I> using type_at_t = typename type_at<L, I>::type;

		// --------------------------------------------------------------------------
		// mp::tl::push_back
		// add type T to L at end of list
		// --------------------------------------------------------------------------
		template <class L, class... Ts> struct push_back;
		template <template <class...> class L, class... Ts, class... Us> struct push_back<L<Us...>, Ts...>
		{
			using type = L<Us..., Ts...>;
		};
		template <class L, class... Ts> using push_back_t = typename push_back<L, Ts...>::type;

		// --------------------------------------------------------------------------
		// mp::tl::push_front
		// add type T to L at top of list
		// --------------------------------------------------------------------------
		template <class L, class... Ts> struct push_front;
		template <template <class...> class L, class... Ts, class... Us> struct push_front<L<Us...>, Ts...>
		{
			using type = L<Ts..., Us...>;
		};
		template <class L, class... Ts> using push_front_t = typename push_front<L, Ts...>::type;

		// --------------------------------------------------------------------------
		// mp::tl::concat
		// concat list L0 and L1
		// --------------------------------------------------------------------------
		template <class L1, class L2> struct concat;
		template <template <class...> class L, class... T0s, class... T1s> struct concat<L<T0s...>, L<T1s...>>
		{
			using type = L<T0s..., T1s...>;
		};
		template <class L0, class L1> using concat_t = typename concat<L0, L1>::type;

		// --------------------------------------------------------------------------
		// mp::tl::erase
		// erase type T from L
		// --------------------------------------------------------------------------
		template <class L, class T> struct erase;
		// list is empty
		template <template <class...> class L, class T> struct erase<L<>, T>
		{
			using type = L<>;
		};
		// class T is head of list L
		template <template <class...> class L, class... Ts, class T> struct erase<L<T, Ts...>, T>
		{
			using type = typename erase<L<Ts...>, T>::type;
		};
		// class T is not head of list L, so call recursively
		template <template <class...> class L, class H, class... Ts, class T> struct erase<L<H, Ts...>, T>
		{
			using type = push_front_t<typename erase<L<Ts...>, T>::type, H>;
		};
		// convenience template to access index type
		template <class L, class T> using erase_t = typename erase<L, T>::type;

		// --------------------------------------------------------------------------
		// mp::tl::unique
		// erase all but one instance of any type T from L
		// --------------------------------------------------------------------------
		template <class L> struct unique;
		// list is empty
		template <template <class...> class L> struct unique<L<>>
		{
			using type = L<>;
		};
		// class T is head of list L
		template <template <class...> class L, class... Ts, class T> struct unique<L<T, Ts...>>
		{
			// private :
			//	using l1 = typename unique< L<Ts...> >::type;
			//	using l2 = erase_t< l1, T >;

			// public :
			//	using type = push_front_t< l2, T >;
			using type = push_front_t<erase_t<typename unique<L<Ts...>>::type, T>, T>;
		};
		// convenience template to access index type
		template <class L> using unique_t = typename unique<L>::type;

		// --------------------------------------------------------------------------
		// mp::tl::contains
		// Tests for existens of type T in L
		// --------------------------------------------------------------------------
		template <class L, class T> struct contains;
		// list is empty
		template <template <class...> class L, class T> struct contains<L<>, T> : public std::false_type
		{
		};
		// class T is head of list L
		template <template <class...> class L, class... Ts, class T> struct contains<L<T, Ts...>, T> : public std::true_type
		{
		};
		// class T is not head of non-empty list L
		template <template <class...> class L, class U, class... Ts, class T> struct contains<L<U, Ts...>, T> : public contains<L<Ts...>, T>
		{
		};
		// convenience template to access index type
		template <class L, class T> constexpr bool contains_v = contains<L, T>::value;
		// template <class L, class T> using contains_v = contains<L, T>::value;
	}
}