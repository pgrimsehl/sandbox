#pragma once

namespace core
{
	/*
	N4618 20.7
	Header <variant> synopsis

	// 20.7.2, variant
	template <class... Types> class variant;

	// 20.7.3, variant helper classes
	template <class T> struct variant_size; // not defined
	template <class T> struct variant_size<const T>;
	template <class T> struct variant_size<volatile T>;
	template <class T> struct variant_size<const volatile T>;
	template <class T> constexpr size_t variant_size_v = variant_size<T>::value;
	template <class... Types> struct variant_size<variant<Types...>>;
	template <size_t I, class T> struct variant_alternative; // not defined
	template <size_t I, class T> struct variant_alternative<I, const T>;
	template <size_t I, class T> struct variant_alternative<I, volatile T>;
	template <size_t I, class T> struct variant_alternative<I, const volatile T>;
	template <size_t I, class T> using variant_alternative_t = typename variant_alternative<I, T>::type;
	template <size_t I, class... Types> struct variant_alternative<I, variant<Types...>>;
	constexpr size_t variant_npos = -1;

	// 20.7.4, value access
	template <class T, class... Types>
	constexpr bool holds_alternative( const variant<Types...> & ) noexcept;
	template <size_t I, class... Types>
	constexpr variant_alternative_t<I, variant<Types...>> &get( variant<Types...> & );
	template <size_t I, class... Types>
	constexpr variant_alternative_t<I, variant<Types...>> &&get( variant<Types...> && );
	template <size_t I, class... Types>
	constexpr const variant_alternative_t<I, variant<Types...>> &get( const variant<Types...> & );
	template <size_t I, class... Types>
	constexpr const variant_alternative_t<I, variant<Types...>> &&get( const variant<Types...> && );
	template <class T, class... Types> constexpr T &			  get( variant<Types...> & );
	template <class T, class... Types> constexpr T &&			  get( variant<Types...> && );
	template <class T, class... Types> constexpr const T &		  get( const variant<Types...> & );
	template <class T, class... Types> constexpr const T &&		  get( const variant<Types...> && );
	template <size_t I, class... Types>
	constexpr std::add_pointer_t<variant_alternative_t<I, variant<Types...>>>
	get_if( variant<Types...> * ) noexcept;
	template <size_t I, class... Types>
	constexpr std::add_pointer_t<const variant_alternative_t<I, variant<Types...>>>
	get_if( const variant<Types...> * ) noexcept;
	template <class T, class... Types>
	constexpr std::add_pointer_t<T> get_if( variant<Types...> * ) noexcept;
	template <class T, class... Types>
	constexpr std::add_pointer_t<const T> get_if( const variant<Types...> * ) noexcept;

	// 20.7.5, relational operators
	template <class... Types>
	constexpr bool operator==( const variant<Types...> &, const variant<Types...> & );
	template <class... Types>
	constexpr bool operator!=( const variant<Types...> &, const variant<Types...> & );
	template <class... Types>
	constexpr bool operator<( const variant<Types...> &, const variant<Types...> & );
	template <class... Types>
	constexpr bool operator>( const variant<Types...> &, const variant<Types...> & );
	template <class... Types>
	constexpr bool operator<=( const variant<Types...> &, const variant<Types...> & );
	template <class... Types>
	constexpr bool operator>=( const variant<Types...> &, const variant<Types...> & );

	// 20.7.6, visitation
	template <class Visitor, class... Variants> constexpr XXX visit( Visitor &&, Variants &&... );

	// 20.7.7, class monostate
	struct monostate;

	// 20.7.8, monostate relational operators
	constexpr bool operator<( monostate, monostate ) noexcept;
	constexpr bool operator>( monostate, monostate ) noexcept;
	constexpr bool operator<=( monostate, monostate ) noexcept;
	constexpr bool operator>=( monostate, monostate ) noexcept;
	constexpr bool operator==( monostate, monostate ) noexcept;
	constexpr bool operator!=( monostate, monostate ) noexcept;

	// 20.7.9, specialized algorithms
	template <class... Types>
	void swap( variant<Types...> &, variant<Types...> & ) noexcept( see below );

	// 20.7.10, class bad_variant_access
	class bad_variant_access;

	// 20.7.11, hash support
	template <class T> struct hash;
	template <class... Types> struct hash<variant<Types...>>;
	template <> struct hash<monostate>;

	// 20.7.12, allocator-related traits
	template <class T, class Alloc> struct uses_allocator;
	template <class... Types, class Alloc> struct uses_allocator<variant<Types...>, Alloc>;
	*/

	// N4618 20.7.2 class template variant
	template <class... Types> class variant
	{
	public:
		// 20.7.2.1, constructors
		constexpr variant() noexcept( see below );
		variant( const variant & );
		variant( variant && ) noexcept( see below );
		template <class T> constexpr variant( T && ) noexcept( see below );
		template <class T, class... Args> constexpr explicit variant( in_place_type_t<T>, Args &&... );
		template <class T, class U, class... Args>
		constexpr explicit variant( in_place_type_t<T>, std::initializer_list<U>, Args &&... );
		template <size_t I, class... Args>
		constexpr explicit variant( in_place_index_t<I>, Args &&... );
		template <size_t I, class U, class... Args>
		constexpr explicit variant( in_place_index_t<I>, std::initializer_list<U>, Args &&... );

		// allocator-extended constructors
		template <class Alloc> variant( std::allocator_arg_t, const Alloc & );
		template <class Alloc> variant( std::allocator_arg_t, const Alloc &, const variant & );
		template <class Alloc> variant( std::allocator_arg_t, const Alloc &, variant && );
		template <class Alloc, class T> variant( std::allocator_arg_t, const Alloc &, T && );
		template <class Alloc, class T, class... Args>
		variant( std::allocator_arg_t, const Alloc &, in_place_type_t<T>, Args &&... );
		template <class Alloc, class T, class U, class... Args>
		variant( std::allocator_arg_t, const Alloc &, in_place_type_t<T>, std::initializer_list<U>, Args &&... );
		template <class Alloc, size_t I, class... Args>
		variant( std::allocator_arg_t, const Alloc &, in_place_index_t<I>, Args &&... );
		template <class Alloc, size_t I, class U, class... Args>
		variant( std::allocator_arg_t, const Alloc &, in_place_index_t<I>, std::initializer_list<U>, Args &&... );

		// 20.7.2.2, destructor
		~variant();

		// 20.7.2.3, assignment
		variant &					operator=( const variant & );
		variant &					operator=( variant && ) noexcept( see below );
		template <class T> variant &operator=( T && ) noexcept( see below );

		// 20.7.2.4, modifiers
		template <class T, class... Args> void			 emplace( Args &&... );
		template <class T, class U, class... Args> void  emplace( std::initializer_list<U>, Args &&... );
		template <size_t I, class... Args> void			 emplace( Args &&... );
		template <size_t I, class U, class... Args> void emplace( std::initializer_list<U>, Args &&... );

		// 20.7.2.5, value status
		constexpr bool   valueless_by_exception() const noexcept;
		constexpr size_t index() const noexcept;

		// 20.7.2.6, swap
		void swap( variant & ) noexcept( see below );
	};
}