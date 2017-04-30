#pragma once

namespace mp
{
	namespace tl
	{
		// --------------------------------------------------------------------------
		// mp::tl::typelist
		// --------------------------------------------------------------------------
		template <class... Types> struct typelist
		{
		};

		// --------------------------------------------------------------------------
		// mp::tl::list_size
		// Size of a typelist (number of template parameters)
		// --------------------------------------------------------------------------
		template <class L> struct list_size;
		template <template <class...> class L, class... Ts> struct list_size<L<Ts...>> : public std::integral_constant<size_t, sizeof...( Ts )>
		{
		};
		// NOTE: variable templates are a feature of C++14
		template <class L> constexpr bool list_size_v = list_size<L>::value;

		// --------------------------------------------------------------------------
		// mp::tl::index_of_type
		// --------------------------------------------------------------------------
		// get index of a type in a type list
		template <class L, class T> struct index_of_type;
		// class V is head of list L, so index is 0
		template <template <class...> class L, class... T, class V> struct index_of_type<L<V, T...>, V>
		{
			using type = std::integral_constant<size_t, 0>;
		};
		// class V is not head of list L, so increment index and call recursively
		template <template <class...> class L, class T1, class... T, class V> struct index_of_type<L<T1, T...>, V>
		{
			using type = std::integral_constant<size_t, 1 + index_of_type<L<T...>, V>::type::value>;
		};
		// convenience template to access inner class type
		template <class L, class V> using index_of_type_t = typename index_of_type<L, V>::type;

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
		// convenience template to access inner class type
		template <class L, size_t I> using type_at_t = typename type_at<L, I>::type;

		// --------------------------------------------------------------------------
		// mp::tl::front_t
		// --------------------------------------------------------------------------
		// get fist type T type list L
		template <class L> using front_t = typename type_at<L, 0>::type;

		// --------------------------------------------------------------------------
		// mp::tl::back_t
		// --------------------------------------------------------------------------
		// get last type T in type list L
		template <class L> struct back;
		template <template <class...> class L, class... Ts>
		struct back<L<Ts...>> : public type_at<L<Ts...>, sizeof...( Ts ) - static_cast<size_t>( 1 )>
		{
		};
		// convenience template to access inner class type
		template <class L> using back_t = typename back<L>::type;

		// --------------------------------------------------------------------------
		// mp::tl::push_back
		// add type T to L at end of list
		// --------------------------------------------------------------------------
		template <class L, class... Ts> struct push_back;
		template <template <class...> class L, class... Ts, class... Us> struct push_back<L<Us...>, Ts...>
		{
			using type = L<Us..., Ts...>;
		};
		// convenience template to access inner class type
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
		// convenience template to access inner class type
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
		// convenience template to access inner class type
		template <class L0, class L1> using concat_t = typename concat<L0, L1>::type;

		// --------------------------------------------------------------------------
		// mp::tl::split_at
		// Splits list L at index I, resulting in list L0 containing indices [0...I-1] and list L1 containing the rest
		// --------------------------------------------------------------------------
		template <class L, size_t I> struct split_at;
		template <template <class...> class L, class... Ts, class T> struct split_at<L<T, Ts...>, 0>
		{
			using type0 = L<>;
			using type1 = L<T, Ts...>;
		};
		template <template <class...> class L, class... Ts, class T, size_t I> struct split_at<L<T, Ts...>, I>
		{
		private:
			using recurse_list = split_at<L<Ts...>, I - static_cast<size_t>(1)>;
		public:
			using type0 = push_front_t<typename recurse_list::type0, T>;
			using type1 = typename recurse_list::type1;

		};
		// convenience template to access inner class type
		template <class L, size_t I> using split_at_t0 = typename split_at<L, I>::type0;
		template <class L, size_t I> using split_at_t1 = typename split_at<L, I>::type1;

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
		// convenience template to access inner class type
		template <class L, class T> using erase_t = typename erase<L, T>::type;

		// --------------------------------------------------------------------------
		// mp::tl::erase_first
		// erase the first occurence of type T from L
		// --------------------------------------------------------------------------
		template <class L, class T> struct erase_first;
		// list is empty
		template <template <class...> class L, class T> struct erase_first<L<>, T>
		{
			using type = L<>;
		};
		// class T is head of list L
		template <template <class...> class L, class... Ts, class T> struct erase_first<L<T, Ts...>, T>
		{
			using type = typename L<Ts...>;
		};
		// class T is not head of list L, so call recursively
		template <template <class...> class L, class H, class... Ts, class T> struct erase_first<L<H, Ts...>, T>
		{
			using type = push_front_t<typename erase_first<L<Ts...>, T>::type, H>;
		};
		// convenience template to access inner class type
		template <class L, class T> using erase_first_t = typename erase_first<L, T>::type;

		// --------------------------------------------------------------------------
		// mp::tl::erase_at
		// --------------------------------------------------------------------------
		// removes type at index from type list L
		template <class L, size_t> struct erase_at;
		// index is 0, remove head of list
		template <template <class...> class L, class... Ts, class T> struct erase_at<L<T, Ts...>, 0>
		{
			using type = L<Ts...>;
		};
		// index is > 0, so call recursively
		template <template <class...> class L, class... Ts, size_t I, class T> struct erase_at<L<T, Ts...>, I>
		{
			static_assert( 0 < sizeof...( Ts ), "index out of bounds" );
			using type = push_front_t<typename erase_at<L<Ts...>, I - 1>::type, T>;
		};
		// convenience template to access inner class type
		template <class L, size_t I> using erase_at_t = typename erase_at<L, I>::type;

		// --------------------------------------------------------------------------
		// mp::tl::insert_at
		// --------------------------------------------------------------------------
		// Inserts type T at index I in type list L, moving element at position I and all
		// subsequent elements by one towards the end of the list
		template <class L, size_t, class T> struct insert_at;
		// index is 0, insert at top of list
		// NOTE: class U is used here to resolve ambiguity with the recursive specialization
		template <template <class...> class L, class... Ts, class T, class U> struct insert_at<L<U, Ts...>, 0, T>
		{
			using type = L<T, U, Ts...>;
		};
		// index is > 0, so call recursively
		template <template <class...> class L, class... Ts, size_t I, class T, class U> struct insert_at<L<U, Ts...>, I, T>
		{
			static_assert( 0 < sizeof...( Ts ), "index out of bounds" );
			using type = push_front_t<typename insert_at<L<Ts...>, I - 1, T>::type, U>;
		};
		// convenience template to access inner class type
		template <class L, size_t I, class T> using insert_at_t = typename insert_at<L, I, T>::type;

		// --------------------------------------------------------------------------
		// mp::tl::pop_front
		// remove type at top of list
		// --------------------------------------------------------------------------
		template <class L> struct pop_front;
		//// empty list will not be changed
		// template <template <class...> class L> struct pop_front<L<>>
		//{
		//	using type = L<>;
		//};
		// T is at top of list
		template <template <class...> class L, class... Ts, class T> struct pop_front<L<T, Ts...>>
		{
			using type = L<Ts...>;
		};
		// convenience template to access inner class type
		template <class L> using pop_front_t = typename pop_front<L>::type;

		// --------------------------------------------------------------------------
		// mp::tl::pop_back
		// remove type at end of list
		// --------------------------------------------------------------------------
		template <class L> struct pop_back;
		//// empty list will not be changed
		// template <template <class...> class L> struct pop_back<L<>>
		//{
		//	using type = L<>;
		//};
		// list has oen element
		template <template <class...> class L, class T> struct pop_back<L<T>>
		{
			using type = L<>;
		};
		// copy all elements but the last one
		template <template <class...> class L, class... Ts, class T> struct pop_back<L<T, Ts...>>
		{
			using type = push_front_t<typename pop_back<L<Ts...>>::type, T>;
		};
		// convenience template to access inner class type
		template <class L> using pop_back_t = typename pop_back<L>::type;

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
		// convenience template to access inner class type
		template <class L> using unique_t = typename unique<L>::type;

		// --------------------------------------------------------------------------
		// mp::tl::contains
		// Tests for existence of type T in L
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
		// convenience template to access class value
		// NOTE: variable templates are a feature of C++14
		template <class L, class T> constexpr bool contains_v = contains<L, T>::value;

		// --------------------------------------------------------------------------
		// mp::tl::inverse
		// Inverses the order of elements in L
		// --------------------------------------------------------------------------
		template <class L> struct inverse;
		// list is empty
		template <template <class...> class L> struct inverse<L<>>
		{
			using type = L<>;
		};
		// list has one element
		template <template <class...> class L, class T> struct inverse<L<T>>
		{
			using type = L<T>;
		};
		// list has two elements
		template <template <class...> class L, class T, class U> struct inverse<L<T, U>>
		{
			using type = L<U, T>;
		};
		// recurse through list with at least 3 elements
		template <template <class...> class L, class... Ts> struct inverse<L<Ts...>>
		{
		private:
			using front_type = front_t<L<Ts...>>;
			using back_type  = back_t<L<Ts...>>;

		public:
			using type = push_front_t<push_back_t<typename inverse<pop_front_t<pop_back_t<L<Ts...>>>>::type, front_type>, back_type>;
		};
		// convenience template to access class value
		template <class L> using inverse_t = typename inverse<L>::type;

		// --------------------------------------------------------------------------
		// mp::tl::replace_at
		// --------------------------------------------------------------------------
		// replaces type at index i with T in type list L
		template <class L, size_t, class T> struct replace_at;
		// index is 0, replace head of list
		template <template <class...> class L, class... Ts, class T, class U> struct replace_at<L<U, Ts...>, 0, T>
		{
			using type = L<T, Ts...>;
		};
		// index is > 0, so call recursively
		template <template <class...> class L, class... Ts, size_t I, class T, class U> struct replace_at<L<U, Ts...>, I, T>
		{
			static_assert( 0 < sizeof...( Ts ), "index out of bounds" );
			using type = push_front_t<typename replace_at<L<Ts...>, I - 1, T>::type, U>;
		};
		// convenience template to access inner class type
		template <class L, size_t I, class T> using replace_at_t = typename replace_at<L, I, T>::type;

		// --------------------------------------------------------------------------
		// mp::tl::replace
		// --------------------------------------------------------------------------
		// replaces all occurrences of type T with type U in type list L
		template <class L, class T, class U> struct replace;
		// list is empty
		template <template <class...> class L, class T, class U> struct replace<L<>, T, U>
		{
			using type = L<>;
		};
		// T is at front, replace T and recurse
		template <template <class...> class L, class... Ts, class T, class U> struct replace<L<T, Ts...>, T, U>
		{
			using type = push_front_t<typename replace<L<Ts...>, T, U>::type, U>;
		};
		// T is not at front, keep type and recurse
		template <template <class...> class L, class... Ts, class T, class U, class V> struct replace<L<V, Ts...>, T, U>
		{
			using type = push_front_t<typename replace<L<Ts...>, T, U>::type, V>;
		};
		// convenience template to access inner class type
		template <class L, class T, class U> using replace_t = typename replace<L, T, U>::type;

		// --------------------------------------------------------------------------
		// mp::tl::replace_first
		// Replaces the first occurrence of type T with type U in type list L
		// --------------------------------------------------------------------------
		template <class L, class T, class U> struct replace_first;
		// list is empty
		template <template <class...> class L, class T, class U> struct replace_first<L<>, T, U>
		{
			using type = L<>;
		};
		// T is at front, replace T and stop
		template <template <class...> class L, class... Ts, class T, class U> struct replace_first<L<T, Ts...>, T, U>
		{
			using type = L<U, Ts...>;
		};
		// T is not at front, keep type and recurse
		template <template <class...> class L, class... Ts, class T, class U, class V> struct replace_first<L<V, Ts...>, T, U>
		{
			using type = push_front_t<typename replace_first<L<Ts...>, T, U>::type, V>;
		};
		// convenience template to access inner class type
		template <class L, class T, class U> using replace_first_t = typename replace_first<L, T, U>::type;

		// --------------------------------------------------------------------------
		// mp::tl::swap
		// Swaps elements at index I and J in typelist L
		// --------------------------------------------------------------------------
		template <class L, size_t I, size_t J> struct swap
		{
		private:
			using j_type = type_at_t<L, J>;
			using l_temp = replace_at_t<L, J, type_at_t<L, I>>;

		public:
			using type = replace_at_t<l_temp, I, j_type>;
		};
		// convenience template to access inner class type
		template <class L, size_t I, size_t J> using swap_t = typename swap<L, I, J>::type;

		// --------------------------------------------------------------------------
		// mp::tl::first_element_of_ordered_list
		// determines the type that would be the first element of the sorted list L
		// when sort predicate P is applied
		// P must be a template with two template type parameters T and U and define an
		// inner type 'type' that contains either T or U
		// --------------------------------------------------------------------------
		template <class L, template <typename, typename> class P> struct first_element_of_ordered_list;
		template <template <class...> class L, class T, template <typename, typename> class P> struct first_element_of_ordered_list<L<T>, P>
		{
			using type = T;
		};
		template <template <class...> class L, class... Ts, class U, class T, template <typename, typename> class P>
		struct first_element_of_ordered_list<L<T, U, Ts...>, P>
		{
			using smaller_type = typename P<T, U>::type;
			using recurse_list = first_element_of_ordered_list<L<smaller_type, Ts...>, P>;

		public:
			using type = typename recurse_list::type;
		};
		template <class L, template <typename, typename> class P>
		using first_element_of_ordered_list_t = typename first_element_of_ordered_list<L, P>::type;

		// --------------------------------------------------------------------------
		// mp::tl::sort
		// Sort the list L using predicate P
		// P must be a template with two template type parameters T and U and define an
		// inner type 'type' that contains either T or U
		// --------------------------------------------------------------------------
		template <class L, template <typename, typename> class P> struct sort;
		template <template <class...> class L, template <typename, typename> class P> struct sort<L<>, P>
		{
			using type = L<>;
		};
		template <template <class...> class L, class... Ts, template <typename, typename> class P> struct sort<L<Ts...>, P>
		{
			using first_element = first_element_of_ordered_list_t<L<Ts...>, P>;
			using next_list		= erase_first_t<L<Ts...>, first_element>;

		public:
			using type = push_front_t<typename sort<next_list, P>::type, first_element>;
		};
		template <class L, template <typename, typename> class P> using sort_t = typename sort<L, P>::type;
	}
}