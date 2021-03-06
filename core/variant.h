#pragma once

#include "utility.h" // in_place_type_t, in_place_index_t
#include <type_traits>

namespace core
{

	constexpr size_t variant_npos = -1;

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
												 !std::is_same<decay_T, variant<Types...>>::value &&
												 std::is_constructible<Ti, T>::value &&
												 !is_specialization_of_in_place_type<decay_T>::value &&
												 !is_specialization_of_in_place_index<decay_T>::value>;
		};

		// test Ti for overload resolution
		template <typename T, typename Ti>
		struct evaluate_Ti : public std::integral_constant<bool, evaluate_Ti_helper<T, Ti>::type::value>
		{
		};

		// helper type for further type selection
		struct call_recursion_t
		{
		};

		//// helper class to select Tj from all Tis
		// template <typename T, typename... Ts> struct select_Tj;
		// template <typename T, typename Ti> struct select_Tj<T, Ti>
		//{
		//	using eval_type = typename std::conditional<evaluate_Ti<T, Ti>::value, Ti, void>::type;
		//	using local_index_type =
		//		typename std::conditional<std::is_same<void, eval_type>::value,
		//								  std::integral_constant<size_t, variant_npos>,
		//								  std::integral_constant<size_t, 1>>::type;
		//	using index_type = local_index_type;
		//	using type		 = eval_type;
		//};
		// template <typename T, typename Ti, typename... Ts> struct select_Tj<T, Ti, Ts...>
		//{
		//	using this_type = select_Tj<T, Ti, Ts...>;
		//	using eval_type = typename std::conditional<
		//		evaluate_Ti<T, Ti>::value,
		//		typename std::conditional<!std::disjunction<evaluate_Ti<T, Ts>...>::value, Ti,
		//								  void>::type,
		//		call_recursion_t>::type;
		//	using selector_type =
		//		typename std::conditional<std::is_same<eval_type, call_recursion_t>::value,
		//								  typename select_Tj<T, Ts...>, this_type>::type;
		//	using local_index_type = typename std::conditional<
		//		std::is_same<selector_type, this_type>::value,
		//		typename std::conditional<std::is_same<void, eval_type>::value,
		//								  std::integral_constant<size_t, variant_npos>,
		//								  std::integral_constant<size_t, sizeof...( Ts ) + 1>>::type,
		//		call_recursion_t>::type;

		//	using index_type =
		//		typename std::conditional<std::is_same<call_recursion_t, local_index_type>::value,
		//								  typename selector_type::local_index_type,
		//								  local_index_type>::type;

		//	using type = typename std::conditional<std::is_same<selector_type, this_type>::value,
		//										   eval_type, typename selector_type::type>::type;

		//	//static_assert( !std::is_same<type, call_recursion_t>::value, "" );
		//};

		// helper class to select Tj from all Tis
		template <typename T, typename... Ts> struct select_Tj;
		template <typename T, typename Ti> struct select_Tj<T, Ti>
		{
			using type = typename std::conditional<evaluate_Ti<T, Ti>::value, Ti, void>::type;
		};
		template <typename T, typename Ti, typename... Ts> struct select_Tj<T, Ti, Ts...>
		{
			using type = typename std::conditional<
				evaluate_Ti<T, Ti>::value,
				typename std::conditional<!std::disjunction<evaluate_Ti<T, Ts>...>::value, Ti,
										  void>::type,
				typename select_Tj<T, Ts...>::type>::type;
			// using index_type = typename std::conditional<
			//	std::is_same<type, void>::value,
			//	std::integral_constant<size_t, variant_npos>,
			//	std::integral_constant<size_t, variant_npos>>::type;
		};

		// helper class to test uniqueness of type T in Ts...
		template <typename T, typename... Ts> struct contains_one_instance : public std::false_type
		{
		};
		template <typename T, typename... Ts>
		struct contains_one_instance<T, T, Ts...>
			: public std::integral_constant<bool, !contains_one_instance<T, Ts...>::value>
		{
		};
		template <typename T, typename U, typename... Ts>
		struct contains_one_instance<T, U, Ts...> : public contains_one_instance<T, Ts...>
		{
		};

		// helper class to compute the (inverse) index of T in Ts...
		template <typename T, typename... Ts> struct index_of;
		template <typename T, typename... Ts>
		struct index_of<T, T, Ts...> : public std::integral_constant<size_t, sizeof...( Ts ) + 1>
		{
		};
		template <typename T, typename U, typename... Ts>
		struct index_of<T, U, Ts...> : public index_of<T, Ts...>
		{
		};

		// storage for in-place construction of all types in Types
		using storage_type = typename std::aligned_union<16, Types...>::type;

		// method table to handle a value of type ValueType, without knowledge about the type
		struct vtable_type
		{
			void ( *copy_construct )( storage_type &_dst, const storage_type &_src );
			void ( *move_construct )( storage_type &_dst, storage_type &_src );
			void ( *copy_assign )( storage_type &_dst, const storage_type &_src );
			void ( *move_assign )( storage_type &_dst, storage_type &_src );
			void ( *swap )( storage_type &_dst, storage_type &_src );
			void ( *destroy )( storage_type &_dst );
		};

		// method table to handle a value of type ValueType, without knowledge about the type
		template <class ValueType> struct typed_vtable_type
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
			// assign value using ValueType's copy assignment operator
			static void copy_assign( storage_type &_dst, const storage_type &_src )
			{
				reinterpret_cast<ValueType &>( _dst ) = reinterpret_cast<const ValueType &>( _src );
			}
			// assign value using ValueType's move assignment operator
			static void move_assign( storage_type &_dst, storage_type &_src )
			{
				reinterpret_cast<ValueType &>( _dst ) =
					std::move( reinterpret_cast<const ValueType &>( _src ) );
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
		template <typename ValueType> struct constructor_type
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

		static_assert( 0 < sizeof...( Types ),
					   "Template parameter list for variant must not be empty" );

		// static_assert( no_duplicates<Types...>::value, "Typelist contains duplicates" );

		// helper method to make the variant valueless
		void make_valueless()
		{
			if ( variant_npos > m_Index )
			{
				m_VTables[ m_Index ].destroy( m_Storage );
				m_Index = variant_npos;
			}
		}

		using T0 = typename std::tuple_element<0, std::tuple<Types...>>::type;

		storage_type	   m_Storage;
		size_t			   m_Index = variant_npos;
		static vtable_type m_VTables[ sizeof...( Types ) ];

	public:
		// --------------------------------------------------------------------------
		// 20.7.2.1, constructors
		// --------------------------------------------------------------------------

		// --------------------------------------------------------------------------
		template <typename = std::enable_if<std::is_nothrow_default_constructible<T0>::value>::type>
		constexpr variant() CORE_NOTHROW_IF( ( std::is_nothrow_default_constructible_v<T0>::value ) )
		{
			new ( static_cast<void *>( &m_Storage ) ) T0();
			m_Index = 0;
		}

		// --------------------------------------------------------------------------
		// template <typename = typename std::enable_if<
		//			  std::conjunction<std::is_copy_constructible<Types>...>::value>::type>
		variant( const variant &_rhs )
		{
			if ( variant_npos != _rhs.index() )
			{
				m_VTables[ _rhs.index() ].copy_construct( m_Storage, _rhs.m_Storage );
			}
			m_Index = _rhs.index();
		}

		// --------------------------------------------------------------------------
		template <typename = typename std::enable_if<
					  std::conjunction<std::is_move_constructible<Types>...>::value>::type>
		variant( variant &&_rhs )
			CORE_NOTHROW_IF( std::conjunction<std::is_nothrow_move_constructible<Types>...>::value )
		{
			if ( variant_npos != _rhs.index() )
			{
				m_VTables[ _rhs.index() ].move_construct( m_Storage, _rhs.m_Storage );
			}
			m_Index = _rhs.index();
		}

		// --------------------------------------------------------------------------
		template <class T, typename selector = typename select_Tj<T, Types...>,
				  typename = std::enable_if<!std::is_same<typename selector::type, void>::value,
											typename selector::type>::type>
		constexpr variant( T &&_value )
			CORE_NOTHROW_IF( ( std::is_nothrow_constructible<typename selector::type, T>::value ) )
		{
			new ( static_cast<void *>( &m_Storage ) ) selector::type( std::forward<T>( _value ) );
			// m_Index = sizeof...( Types ) - selector::index_type::value;
			m_Index = sizeof...( Types ) - index_of<selector::type, Types...>::value;
		}

		// --------------------------------------------------------------------------
		template <class T, class... Args,
				  typename = std::enable_if<contains_one_instance<T, Types...>::value &&
											std::is_constructible<T, Args...>::value>::type>
		constexpr explicit variant( in_place_type_t<T>, Args &&... _args )
		{
			new ( static_cast<void *>( &m_Storage ) ) T( std::forward<Args>( _args )... );
			m_Index = sizeof...( Types ) - index_of<T, Types...>::value;
		}

		// --------------------------------------------------------------------------
		template <class T, class U, class... Args,
				  typename = std::enable_if<
					  contains_one_instance<T, Types...>::value &&
					  std::is_constructible<T, std::initializer_list<U> &, Args...>::value>::type>
		constexpr explicit variant( in_place_type_t<T>, std::initializer_list<U> _il, Args &&... _args )
		{
			new ( static_cast<void *>( &m_Storage ) ) T( _il, std::forward<Args>( _args )... );
			m_Index = sizeof...( Types ) - index_of<T, Types...>::value;
		}

		// --------------------------------------------------------------------------
		template <size_t I, class... Args,
				  typename = std::enable_if<
					  ( sizeof...( Types ) > I ) &&
					  std::is_constructible<typename std::tuple_element<I, std::tuple<Types...>>::type,
											Args...>::value>::type>
		constexpr explicit variant( in_place_index_t<I>, Args &&... _args )
		{
			using T = typename std::tuple_element<I, std::tuple<Types...>>::type;
			new ( static_cast<void *>( &m_Storage ) ) T( std::forward<Args>( _args )... );
			m_Index = I;
		}

		// --------------------------------------------------------------------------
		template <size_t I, class U, class... Args,
				  typename = std::enable_if<
					  ( sizeof...( Types ) > I ) &&
					  std::is_constructible<typename std::tuple_element<I, std::tuple<Types...>>::type,
											std::initializer_list<U> &, Args...>::value>::type>
		constexpr explicit variant( in_place_index_t<I>, std::initializer_list<U> _il,
									Args &&... _args )
		{
			using T = typename std::tuple_element<I, std::tuple<Types...>>::type;
			new ( static_cast<void *>( &m_Storage ) ) T( _il, std::forward<Args>( _args )... );
			m_Index = I;
		}

		// --------------------------------------------------------------------------
		// allocator-extended constructors
		// --------------------------------------------------------------------------

		// --------------------------------------------------------------------------
		template <class Alloc> variant( std::allocator_arg_t, const Alloc & );
		// --------------------------------------------------------------------------
		template <class Alloc> variant( std::allocator_arg_t, const Alloc &, const variant & );
		// --------------------------------------------------------------------------
		template <class Alloc> variant( std::allocator_arg_t, const Alloc &, variant && );
		// --------------------------------------------------------------------------
		template <class Alloc, class T> variant( std::allocator_arg_t, const Alloc &, T && );
		// --------------------------------------------------------------------------
		template <class Alloc, class T, class... Args>
		variant( std::allocator_arg_t, const Alloc &, in_place_type_t<T>, Args &&... );
		// --------------------------------------------------------------------------
		template <class Alloc, class T, class U, class... Args>
		variant( std::allocator_arg_t, const Alloc &, in_place_type_t<T>, std::initializer_list<U>,
				 Args &&... );
		// --------------------------------------------------------------------------
		template <class Alloc, size_t I, class... Args>
		variant( std::allocator_arg_t, const Alloc &, in_place_index_t<I>, Args &&... );
		// --------------------------------------------------------------------------
		template <class Alloc, size_t I, class U, class... Args>
		variant( std::allocator_arg_t, const Alloc &, in_place_index_t<I>, std::initializer_list<U>,
				 Args &&... );

		// --------------------------------------------------------------------------
		// 20.7.2.2, destructor
		// --------------------------------------------------------------------------
		// TODO: Ensure that
		// Remarks: If is_trivially_destructible_v<Ti> == true for all Ti then this destructor shall be
		// a trivial destructor.
		~variant()
		{
			if ( variant_npos != m_Index )
			{
				m_VTables[ m_Index ].destroy( m_Storage );
				m_Index = variant_npos;
			}
		}

		// --------------------------------------------------------------------------
		// 20.7.2.3, assignment
		// --------------------------------------------------------------------------

		// --------------------------------------------------------------------------
		variant &operator=( const variant &_rhs )
		{
			variant( _rhs ).swap( *this );
			return *this;
		}

		// --------------------------------------------------------------------------
		variant &operator=( variant &&_rhs )
			CORE_NOTHROW_IF( std::is_nothrow_move_constructible_v<Types>...
								 &&std::is_nothrow_move_assignable_v<Types>... )
		{
			variant( std::move( _rhs ) ).swap( *this );
			return *this;
		}

		// --------------------------------------------------------------------------
		template <class T, typename selector = typename select_Tj<T, Types...>,
				  typename = std::enable_if<!std::is_same<typename selector::type, void>::value,
											typename selector::type>::type>
		variant &operator=( T &&_value )
			CORE_NOTHROW_IF( ( std::is_nothrow_constructible<typename selector::type, T>::value ) )
		{
			variant( std::move( _value ) ).swap( *this );
			return *this;
		}

		// --------------------------------------------------------------------------
		// 20.7.2.4, modifiers
		// --------------------------------------------------------------------------
		template <class T, class... Args,
				  typename = std::enable_if<contains_one_instance<T, Types...>::value &&
											std::is_constructible<T, Args...>::value>::type>
		void emplace( Args &&... _args )
		{
			make_valueless();
			new ( static_cast<void *>( &m_Storage ) ) T( std::forward<Args>( _args )... );
			m_Index = sizeof...( Types ) - index_of<T, Types...>::value;
		}

		// --------------------------------------------------------------------------
		template <class T, class U, class... Args,
				  typename = std::enable_if<
					  contains_one_instance<T, Types...>::value &&
					  std::is_constructible<T, std::initializer_list<U> &, Args...>::value>::type>
		void emplace( std::initializer_list<U> _il, Args &&... _args )
		{
			make_valueless();
			new ( static_cast<void *>( &m_Storage ) ) T( _il, std::forward<Args>( _args )... );
			m_Index = sizeof...( Types ) - index_of<T, Types...>::value;
		}

		// --------------------------------------------------------------------------
		template <size_t I, class... Args,
				  typename = std::enable_if<
					  ( sizeof...( Types ) > I ) &&
					  std::is_constructible<typename std::tuple_element<I, std::tuple<Types...>>::type,
											Args...>::value>::type>
		void emplace( Args &&... _args )
		{
			make_valueless();
			using T = typename std::tuple_element<I, std::tuple<Types...>>::type;
			new ( static_cast<void *>( &m_Storage ) ) T( std::forward<Args>( _args )... );
			m_Index = I;
		}

		// --------------------------------------------------------------------------
		template <size_t I, class U, class... Args,
				  typename = std::enable_if<
					  ( sizeof...( Types ) > I ) &&
					  std::is_constructible<typename std::tuple_element<I, std::tuple<Types...>>::type,
											std::initializer_list<U> &, Args...>::value>::type>
		void emplace( std::initializer_list<U> _il, Args &&... _args )
		{
			make_valueless();
			using T = typename std::tuple_element<I, std::tuple<Types...>>::type;
			new ( static_cast<void *>( &m_Storage ) ) T( _il, std::forward<Args>( _args )... );
			m_Index = I;
		}

		// --------------------------------------------------------------------------
		// 20.7.2.5, value status
		// --------------------------------------------------------------------------
		constexpr bool valueless_by_exception() const noexcept
		{
			return variant_npos == m_Index;
		}

		// --------------------------------------------------------------------------
		constexpr size_t index() const CORE_NOTHROW
		{
			return m_Index;
		}

		// --------------------------------------------------------------------------
		// 20.7.2.6, swap
		// TODO: check correct order of statements to meet the defined state criteria
		// after an exception occurred during construction
		// --------------------------------------------------------------------------
		void swap( variant &_rhs )
			CORE_NOTHROW_IF( std::conjunction<std::is_move_constructible<Types>... /*,
											  std::is_nothrow_swappable<Types>...*/>::value )
		{
			static_assert( std::conjunction<std::is_move_constructible<Types>...>::value,
						   "All value types must be move constructible." );
			/*static_assert( std::conjunction<std::is_nothrow_swappable<Types>...>::value,
						   "All value types must be no-throw-swappable." );*/

			// no self-swapping
			if ( this == &_rhs )
			{
				return;
			}

			// no values to swap
			if ( ( variant_npos == index() ) && ( variant_npos == _rhs.index() ) )
			{
				return;
			}

			// other variant holds no value: move our value to _rhs
			if ( variant_npos == _rhs.index() )
			{
				m_VTables[ m_Index ].move_construct( _rhs.m_Storage, m_Storage );
				_rhs.m_Index = m_Index;
				m_Index		 = variant_npos;
				return;
			}
			// we hold no value: move _rhs value to us
			else if ( variant_npos == index() )
			{
				m_VTables[ _rhs.index() ].move_construct( m_Storage, _rhs.m_Storage );
				m_Index		 = _rhs.index();
				_rhs.m_Index = variant_npos;
				return;
			}
			// we hold the same value type: use swap of contained type
			else if ( _rhs.index() == index() )
			{
				m_VTables[ _rhs.index() ].swap( m_Storage, _rhs.m_Storage );
				return;
			}
			// we hold different value types
			else
			{
				variant temp( std::move( *this ) );
				m_VTables[ _rhs.index() ].move_construct( m_Storage, _rhs.m_Storage );
				m_Index = _rhs.index();
				m_VTables[ temp.index() ].move_construct( _rhs.m_Storage, temp.m_Storage );
				_rhs.m_Index = temp.index();
				temp.m_Index = variant_npos;
			}
		}
	};

	template <typename... Types>
	typename variant<Types...>::vtable_type variant<Types...>::m_VTables[ sizeof...( Types ) ] = {
		{ &typed_vtable_type<Types>::copy_construct, &typed_vtable_type<Types>::move_construct,
		  &typed_vtable_type<Types>::copy_assign, &typed_vtable_type<Types>::move_assign,
		  &typed_vtable_type<Types>::swap, &typed_vtable_type<Types>::destroy }...
	};
}