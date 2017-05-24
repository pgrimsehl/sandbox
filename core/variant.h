#pragma once

#include "utility.h" // in_place_type_t, in_place_index_t
#include <type_traits>

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
		variant( std::allocator_arg_t, const Alloc &, in_place_type_t<T>, std::initializer_list<U>,
				 Args &&... );
		template <class Alloc, size_t I, class... Args>
		variant( std::allocator_arg_t, const Alloc &, in_place_index_t<I>, Args &&... );
		template <class Alloc, size_t I, class U, class... Args>
		variant( std::allocator_arg_t, const Alloc &, in_place_index_t<I>, std::initializer_list<U>,
				 Args &&... );

		// 20.7.2.2, destructor
		~variant();

		// 20.7.2.3, assignment
		variant &					operator=( const variant & );
		variant &					operator=( variant && ) noexcept( see below );
		template <class T> variant &operator=( T && ) noexcept( see below );

		// 20.7.2.4, modifiers
		template <class T, class... Args> void			emplace( Args &&... );
		template <class T, class U, class... Args> void emplace( std::initializer_list<U>, Args &&... );
		template <size_t I, class... Args> void			emplace( Args &&... );
		template <size_t I, class U, class... Args>
		void emplace( std::initializer_list<U>, Args &&... );

		// 20.7.2.5, value status
		constexpr bool   valueless_by_exception() const noexcept;
		constexpr size_t index() const noexcept;

		// 20.7.2.6, swap
		void swap( variant & ) noexcept( see below );

	private:
		// storage for in-place construction of all types in Types
		using storage_type = std::aligned_union<8, Types...>;

		// method table to handle a value of type ValueType, without knowledge about the type
		template <class ValueType> struct vtable_type
		{
			// construct a new object using ValueType's copy constructor
			static void copy_construct( storage_type &_dst, const storage_type &_src )
			{
				new ( &_dst ) ValueType( reinterpret_cast<const ValueType &>( _src ) );
			}
			// construct a new object using ValueType's move constructor
			static void move_construct( storage_type &_dst, storage_type &_src )
			{
				new ( &_dst ) ValueType( std::move( reinterpret_cast<ValueType &>( _src ) ) );
				destroy( _src );
			}
			// exchange values using std::swap
			static void swap( storage_type &_dst, storage_type &_src )
			{
				std::swap( reinterpret_cast<ValueType &>( _dst ),
						   reinterpret_cast<ValueType &>( _src ) );
			}
			// explicit call to ValueType's destructor
			static void destroy( storage_type &_dst )
			{
				reinterpret_cast<ValueType &>( _dst ).~ValueType();
			}
		};

		// construction of type ValueType
		template <typename ValueTypeT> struct constructor_type
		{
			// constructor overload
			template <class... Args> static void construct( storage_type &_storage, Args &&... _args )
			{
				new ( &_storage ) ValueType( std::forward<Args>( _args )... );
			}

			// constructor using initializer list
			template <class U, class... Args>
			static void construct_il( storage_type &_storage, std::initializer_list<U> _il,
									  Args &&... _args )
			{
				new ( &_storage ) ValueType( _il, std::forward<Args>( _args )... );
			}
		};

		// casting of type ValueType
		template <class ValueType> struct cast_type
		{
			static ValueType *cast( storage_type &_storage )
			{
				return reinterpret_cast<ValueType *>( &_storage );
			}
			static const ValueType *cast( const storage_type &_storage )
			{
				return reinterpret_cast<const ValueType *>( &_storage );
			}
		};

		// helper class for the imaginary function FUN(Ti) (N4618 20.7.2.1 - 12)
		// T_j is selected when FUN( std::forward<T>(t) ) is well-formed
		// std::forward returns an rvalue reference T&&, so does makeT()
		template <typename T, typename Ti> struct FUN_Ti
		{
		private:
			static T &&			   makeT();
			static std::true_type  FUN( Ti );
			static std::false_type FUN( ... );

		public:
			using type =
				std::integral_constant<bool,
									   std::is_same<std::true_type, decltype( FUN( makeT() ) )>::value>;
		};

		// helper struct for detection of specialization of in_place_type_t
		template <typename T> struct is_specialization_of_in_place_type : public std::false_type
		{
		};
		template <typename T>
		struct is_specialization_of_in_place_type<in_place_type_t<T>> : public std::true_type
		{
		};

		// helper struct for detection of specialization of in_place_index_t
		template <typename T> struct is_specialization_of_in_place_index : public std::false_type
		{
		};

		template <size_t I>
		struct is_specialization_of_in_place_index<in_place_index_t<I>> : public std::true_type
		{
		};

		// helper class to evaluate T_j for overload resolution (N4618 20.7.2.1 - 16)
		template <typename T, typename Ti> struct evaluate_Ti_helper
		{
			using decay_T	 = typename std::decay<T>::type;
			using FUN_Ti_type = typename FUN_Ti<T, Ti>::type;
			using type =
				std::integral_constant<bool, FUN_Ti_type::value &&
												 !std::is_same<decay_T, variant>::value &&
												 std::is_constructible<Ti, T>::value &&
												 !is_specialization_of_in_place_type<decay_T>::value &&
												 !is_specialization_of_in_place_index<decay_T>::value>;
		};

		// test Ti for overload resolution
		template <typename T, typename Ti>
		struct evaluate_Ti : public std::integral_constant<bool, evaluate_Ti_helper<T, Ti>::type::value>
		{
		};

		// helper class to select Tj from all Tis 
		template <typename T, typename ...Ts> struct select_Tj;

		//template <typename T> struct select_Tj<T>
		//{
		//	using type = void;
		//};

		//template <typename T, typename Ti, typename ...Ts> struct select_Tj< T, Ti, Ts...>
		//{
		//	using type = typename std::conditional< evaluate_Ti<T, Ti>::value,
		//		! std::disjunction<evaluate_Ti<T, Ts>...>::type::value
		//};

	};
}