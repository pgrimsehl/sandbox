#pragma once

#include "core.h"

#include <type_traits> // std::aligned_union

// partial implementation of std::any for C++11
// (See http://open-std.org/JTC1/SC22/WG21/docs/papers/2016/n4618.pdf, 20.8 Storage for any type,
// for specification)
namespace core
{
	class any final
	{
	public:
		// ---------------------------------------------------------------------------
		constexpr any() CORE_NOTHROW : m_VTable( nullptr )
		{
		}

		// ---------------------------------------------------------------------------
		any( const any &_other )
		{
			if ( _other.has_value() )
			{
				_other.m_VTable->copy_construct( m_Storage, _other.m_Storage );
				m_VTable = _other.m_VTable;
			}
		}

		// ---------------------------------------------------------------------------
		any( any &&_other ) CORE_NOTHROW
		{
			if ( _other.has_value() )
			{
				_other.m_VTable->move_construct( m_Storage, _other.m_Storage );
				_other.m_VTable = nullptr;
			}
		}

		// ---------------------------------------------------------------------------
		template <class ValueType,
				  // (N4618 20.8.3.1-9)
				  typename = typename std::enable_if<
					  !std::is_same<typename std::decay<ValueType>::type, any>::value &&
					  std::is_copy_constructible<typename std::decay<ValueType>::type>::value>::type>
		any( ValueType &&_value )
		{
			construct<ValueType, ValueType>( std::forward<ValueType>( _value ) );
		}

		// ---------------------------------------------------------------------------
		~any()
		{
			// (N4618 20.8.3.1-23)
			reset();
		}

		// ---------------------------------------------------------------------------
		any &operator=( const any &_rhs )
		{
			// (N4618 20.8.3.2-1)
			any( _rhs ).swap( *this );
			return *this;
		}

		// ---------------------------------------------------------------------------
		any &operator=( any &&_rhs ) CORE_NOTHROW
		{
			// (N4618 20.8.3.2-4)
			any( std::move( _rhs ) ).swap( *this );
			return *this;
		}

		// ---------------------------------------------------------------------------
		template <typename ValueType> any &operator=( ValueType &&_rhs )
		{
			// (N4618 20.8.3.2-9)
			any( std::forward<ValueType>( _rhs ) ).swap( *this );
			return *this;
		}

		// ---------------------------------------------------------------------------
		template <
			class ValueType, class... Args,
			// (N4618 20.8.3.3-6)
			typename = typename std::enable_if<
				std::is_copy_constructible<typename std::decay<ValueType>::type>::value &&
				std::is_constructible<typename std::decay<ValueType>::type, Args...>::value>::type>
		void emplace( Args &&... _args )
		{
			reset();
			construct<ValueType, Args...>( std::forward<Args>( _args )... );
		}

		// ---------------------------------------------------------------------------
		template <class ValueType, class U, class... Args,
				  // (N4618 20.8.3.3-12)
				  typename = typename std::enable_if<
					  std::is_copy_constructible<typename std::decay<ValueType>::type>::value &&
					  std::is_constructible<typename std::decay<ValueType>::type,
											std::initializer_list<U> &, Args...>::value>::type>
		void emplace( std::initializer_list<U> _il, Args &&... _args )
		{
			reset();
			construct_il<ValueType, U, Args...>( _il, std::forward<Args>( _args )... );
		}

		// ---------------------------------------------------------------------------
		void reset() CORE_NOTHROW
		{
			// (N4618 20.8.3.3-13)
			if ( has_value() )
			{
				m_VTable->destroy( m_Storage );
				m_VTable = nullptr;
			}
		}

		// ---------------------------------------------------------------------------
		void swap( any &_rhs ) CORE_NOTHROW
		{
			if ( this != &_rhs )
			{
				any other_copy( std::move( _rhs ) );
				if ( nullptr != m_VTable )
				{
					m_VTable->move_construct( _rhs.m_Storage, m_Storage );
					_rhs.m_VTable = m_VTable;
					m_VTable	  = nullptr;
				}
				if ( nullptr != other_copy.m_VTable )
				{
					other_copy.m_VTable->move_construct( m_Storage, other_copy.m_Storage );
					m_VTable			= other_copy.m_VTable;
					other_copy.m_VTable = nullptr;
				}
			}
		}

		// ---------------------------------------------------------------------------
		bool has_value() const CORE_NOTHROW
		{
			return ( nullptr != m_VTable );
		}

		// ---------------------------------------------------------------------------
		const type_info &type() const CORE_NOTHROW
		{
			if ( has_value() )
			{
				return m_VTable->type();
			}
			return type_id<void>();
		}

	private:
		// internal struct for most implementation details
		struct internal
		{
			// the internal storage is a union to a pointer to a heap allocated object
			// and a local variable that holds a certain amount of space to allocate
			// small objects
			struct storage
			{
				constexpr storage(){};

				union {
					void *												   heap = nullptr;
					std::aligned_union<sizeof( void * ), void * [2]>::type local;
				};
			};

			// this is the function table definition that will hold the method pointers
			struct vtable_type
			{
				const type_info &( *type )();
				void ( *copy_construct )( storage &_dst, const storage &_src );
				void ( *move_construct )( storage &_dst, storage &_src );
				void ( *swap )( storage &_dst, storage &_src );
				void ( *destroy )( storage &_dst );
			};

			// this is the type-dependent method set for locally allocated values
			template <class T> struct local_storage
			{
				static const type_info &type()
				{
					return type_id<T>();
				}
				// construct a new object using T's copy constructor
				static void copy_construct( storage &_dst, const storage &_src )
				{
					new ( &_dst.local ) T( reinterpret_cast<const T &>( _src.local ) );
				}
				// construct a new object using T's move constructor
				static void move_construct( storage &_dst, storage &_src )
				{
					new ( &_dst.local ) T( std::move( reinterpret_cast<T &>( _src.local ) ) );
					destroy( _src );
				}
				// exchange values using std::swap
				static void swap( storage &_dst, storage &_src )
				{
					std::swap( reinterpret_cast<T &>( _dst.local ),
							   reinterpret_cast<T &>( _src.local ) );
				}
				// explicit call to T's destructor
				static void destroy( storage &_dst )
				{
					reinterpret_cast<T &>( _dst.local ).~T();
				}
			};

			// this is the type-dependent method set for heap allocated values
			template <class T> struct heap_storage
			{
				static const type_info &type()
				{
					return type_id<T>();
				}
				// construct a new object on the heap using T's copy constructor
				static void copy_construct( storage &_dst, const storage &_src )
				{
					_dst.heap = new T( *reinterpret_cast<const T *>( _src.heap ) );
				}
				// move the the heap pointer
				static void move_construct( storage &_dst, storage &_src )
				{
					_dst.heap = _src.heap;
					_src.heap = nullptr;
				}
				// exchange the heap pointers
				static void swap( storage &_dst, storage &_src )
				{
					std::swap( _dst.heap, _src.heap );
				}
				// delete the heap object
				static void destroy( storage &_dst )
				{
					delete reinterpret_cast<T *>( _dst.heap );
					_dst.heap = nullptr;
				}
			};

			// helper template to determine if a value should be allocated on the heap
			// N4618 20.8.3-3: 'Such small-object optimization shall only be applied to types T for
			// which is_nothrow_move_constructible_v<T> is true.'
			template <class T>
			struct allocate_on_heap
				: public std::integral_constant<
					  bool, !std::is_nothrow_move_constructible<T>::value ||
								( sizeof( T ) > sizeof( storage::local ) ) ||
								( std::alignment_of<T>::value >
								  std::alignment_of<decltype( storage::local )>::value )>
			{
			};

			// construction of local values
			template <class ValueType, class T> struct local_construction
			{
				template <class... Args> static void construct( storage &_storage, Args &&... _args )
				{
					new ( &_storage.local ) T( std::forward<Args>( _args )... );
				}
				template <class U, class... Args>
				static void construct_il( storage &_storage, std::initializer_list<U> _il,
										  Args &&... _args )
				{
					new ( &_storage.local ) T( _il, std::forward<Args>( _args )... );
				}
			};
			// construction of heap values
			template <class ValueType, class T> struct heap_construction
			{
				template <class... Args> static void construct( storage &_storage, Args &&... _args )
				{
					_storage.heap = new T( std::forward<Args>( _args )... );
				}
				template <class U, class... Args>
				static void construct_il( storage &_storage, std::initializer_list<U> _il,
										  Args &&... _args )
				{
					_storage.heap = new T( _il, std::forward<Args>( _args )... );
				}
			};

			// casting of local values
			template <class ValueType> struct local_cast
			{
				static ValueType *cast( storage &_storage )
				{
					return reinterpret_cast<ValueType *>( &_storage.local );
				}
				static const ValueType *cast( const storage &_storage )
				{
					return reinterpret_cast<const ValueType *>( &_storage.local );
				}
			};

			// casting of heap values
			template <class ValueType> struct heap_cast
			{
				static ValueType *cast( storage &_storage )
				{
					return reinterpret_cast<ValueType *>( _storage.heap );
				}
				static const ValueType *cast( const storage &_storage )
				{
					return reinterpret_cast<const ValueType *>( _storage.heap );
				}
			};
		};

		// ---------------------------------------------------------------------------
		// creates the internal representation of _value
		// ---------------------------------------------------------------------------
		template <class ValueType, class... Args> void construct( Args &&... _args )
		{
			// for all further processing, we use the decay type T of ValueType
			using T = typename std::decay<ValueType>::type;
			// select heap or local storage
			using storage_type = typename std::conditional<internal::allocate_on_heap<T>::value,
														   typename internal::heap_storage<T>,
														   typename internal::local_storage<T>>::type;
			// select heap or local construction
			using construct_type =
				typename std::conditional<internal::allocate_on_heap<T>::value,
										  typename internal::heap_construction<ValueType, T>,
										  typename internal::local_construction<ValueType, T>>::type;

			// the static vtable for this type
			static internal::vtable_type vtable = { storage_type::type, storage_type::copy_construct,
													storage_type::move_construct, storage_type::swap,
													storage_type::destroy };
			// construct the value
			construct_type::construct<Args...>( m_Storage, std::forward<Args>( _args )... );
			// set the vtable pointer
			m_VTable = &vtable;
		};

		// ---------------------------------------------------------------------------
		// creates the internal representation of _value
		// ---------------------------------------------------------------------------
		template <class ValueType, class U, class... Args>
		void construct_il( std::initializer_list<U> _il, Args &&... _args )
		{
			// for all further processing, we use the decay type T of ValueType
			using T = typename std::decay<ValueType>::type;
			// select heap or local storage
			using storage_type = typename std::conditional<internal::allocate_on_heap<T>::value,
														   typename internal::heap_storage<T>,
														   typename internal::local_storage<T>>::type;
			// select heap or local construction
			using construct_type =
				typename std::conditional<internal::allocate_on_heap<T>::value,
										  typename internal::heap_construction<ValueType, T>,
										  typename internal::local_construction<ValueType, T>>::type;

			// the static vtable for this type
			static internal::vtable_type vtable = { storage_type::type, storage_type::copy_construct,
													storage_type::move_construct, storage_type::swap,
													storage_type::destroy };
			// construct the value
			construct_type::construct_il<U, Args...>( m_Storage, _il, std::forward<Args>( _args )... );
			// set the vtable pointer
			m_VTable = &vtable;
		};

		// ---------------------------------------------------------------------------
		template <class ValueType> ValueType *cast()
		{
			using T			= typename std::decay<ValueType>::type;
			using cast_type = typename std::conditional<internal::allocate_on_heap<T>::value,
														typename internal::heap_cast<ValueType>,
														typename internal::local_cast<ValueType>>::type;
			return cast_type::cast( m_Storage );
		}

		// ---------------------------------------------------------------------------
		template <class ValueType> const ValueType *cast() const
		{
			using T			= typename std::decay<ValueType>::type;
			using cast_type = typename std::conditional<internal::allocate_on_heap<T>::value,
														typename internal::heap_cast<ValueType>,
														typename internal::local_cast<ValueType>>::type;
			return cast_type::cast( m_Storage );
		}

		// the value storage
		internal::storage m_Storage;
		// the pointer to the method table
		internal::vtable_type *m_VTable = nullptr;

		// any_cast must be a friend
		template <class ValueType> friend const ValueType *any_cast( const any *_operand ) CORE_NOTHROW;
		template <class ValueType> friend ValueType *	  any_cast( any *_operand ) CORE_NOTHROW;
	};

	// ---------------------------------------------------------------------------
	// N4618 20.8.2, class bad_any_cast
	// ---------------------------------------------------------------------------
	class bad_any_cast : public std::bad_cast
	{
	public:
		const char *what() const CORE_NOTHROW override
		{
			return "bad any cast";
		}
	};

	// ---------------------------------------------------------------------------
	// swap
	// ---------------------------------------------------------------------------
	inline void swap( any &_x, any &_y ) CORE_NOTHROW
	{
		// N4618 20.8.4-1
		_x.swap( _y );
	}

	// ---------------------------------------------------------------------------
	// template <class T, class... Args> any		   make_any( Args &&... args );

	// ---------------------------------------------------------------------------
	// template <class T, class U, class... Args> any make_any( initializer_list<U> il, Args &&... args
	// );

	// ---------------------------------------------------------------------------
	template <class ValueType> ValueType any_cast( const any &_operand )
	{
		static_assert(
			std::is_reference<ValueType>::value || std::is_copy_constructible<ValueType>::value,
			"Requires: is_reference_v<ValueType> is true or is_copy_constructible_v<ValueType> is true." );
		auto result =
			any_cast<typename std::add_const<typename std::remove_reference<ValueType>::type>::type>(
				&_operand );
		if ( nullptr == result )
		{
			CORE_RAISE( bad_any_cast() );
		}
		return *result;
	}

	// ---------------------------------------------------------------------------
	template <class ValueType> ValueType any_cast( any &_operand )
	{
		static_assert(
			std::is_reference<ValueType>::value || std::is_copy_constructible<ValueType>::value,
			"Requires: is_reference_v<ValueType> is true or is_copy_constructible_v<ValueType> is true." );
		auto result = any_cast<typename std::remove_reference<ValueType>::type>( &_operand );
		if ( nullptr == result )
		{
			CORE_RAISE( bad_any_cast() );
		}
		return *result;
	}

	// ---------------------------------------------------------------------------
	template <class ValueType> ValueType any_cast( any &&_operand )
	{
		static_assert(
			std::is_reference<ValueType>::value || std::is_copy_constructible<ValueType>::value,
			"Requires: is_reference_v<ValueType> is true or is_copy_constructible_v<ValueType> is true." );
		auto result = any_cast<typename std::remove_reference<ValueType>::type>( &_operand );
		if ( nullptr == result )
		{
			CORE_RAISE( bad_any_cast() );
		}
		return *result;
	}

	// ---------------------------------------------------------------------------
	template <class ValueType> const ValueType *any_cast( const any *_operand ) CORE_NOTHROW
	{
		if ( ( nullptr != _operand ) && ( _operand->type() == type_id<ValueType>() ) )
		{
			return _operand->cast<ValueType>();
		}
		return nullptr;
	}

	// ---------------------------------------------------------------------------
	template <class ValueType> ValueType *any_cast( any *_operand ) CORE_NOTHROW
	{
		if ( ( nullptr != _operand ) && ( _operand->type() == type_id<ValueType>() ) )
		{
			return _operand->cast<ValueType>();
		}
		return nullptr;
	}
}