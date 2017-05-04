#pragma once

#include <type_traits> // std::aligned_union
#include <typeinfo>	// std::bad_cast

// partial implementation of std::any for C++11
// (See http://open-std.org/JTC1/SC22/WG21/docs/papers/2016/n4618.pdf, 20.8 Storage for any type, for specification)

namespace core
{
	// N4618 20.8.2, class bad_any_cast
	class bad_any_cast;
	// N4618 20.8.3, class any
	class any;
	// N4618 20.8.4, non-member functions
	void swap( any &_x, any &_y ) noexcept;
	// template <class T, class... Args> any		   make_any( Args &&... args );
	// template <class T, class U, class... Args> any make_any( initializer_list<U> il, Args &&... args );
	template <class ValueType> ValueType		any_cast( const any &_operand );
	template <class ValueType> ValueType		any_cast( any &_operand );
	template <class ValueType> ValueType		any_cast( any &&_operand );
	template <class ValueType> const ValueType *any_cast( const any *_operand ) noexcept;
	template <class ValueType> ValueType *		any_cast( any *_operand ) noexcept;

	class any final
	{
	public:
		// 20.8.3.1, construction and destruction
		constexpr any() noexcept;
		any( const any &_other );
		any( any &&_other ) noexcept;
		template <class ValueType, typename = typename std::enable_if<!std::is_same<typename std::decay<ValueType>::type, any>::value>::type>
		any( ValueType &&_value );
		// template <class ValueType, class... Args> explicit any( in_place_type_t<ValueType>, Args &&... );
		// template <class ValueType, class U, class... Args> explicit any( in_place_type_t<ValueType>, initializer_list<U>, Args &&... );
		~any();

		// 20.8.3.2, assignments
		any &							operator=( const any &_rhs );
		any &							operator=( any &&_rhs ) noexcept;
		template <class ValueType> any &operator=( ValueType &&_rhs );

		// 20.8.3.3, modifiers
		// template <class ValueType, class... Args> void			emplace( Args &&... );
		// template <class ValueType, class U, class... Args> void emplace( initializer_list<U>, Args &&... );
		void reset() noexcept;
		void swap( any &_rhs ) noexcept;

		// 20.8.3.4, observers
		bool			 has_value() const noexcept;
		const type_info &type() const noexcept;

	private:
		// internal struct for most implementation details
		struct internal
		{
			// the internal storage is a union to a pointer to a heap allocated object
			// and a local variable that holds a certain amount of space to allocate
			// small objects
			struct storage
			{
				union {
					void *												   heap;
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
					return typeid( T );
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
					std::swap( reinterpret_cast<T &>( _dst.local ), reinterpret_cast<T &>( _src.local ) );
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
					return typeid( T );
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
			// N4618 20.8.3-3: 'Such small-object optimization shall only be applied to types T for which is_nothrow_move_constructible_v<T> is true.'
			template <class T>
			struct allocate_on_heap
				: public std::integral_constant<bool, !std::is_nothrow_move_constructible<T>::value || ( sizeof( T ) > sizeof( storage::local ) ) ||
														  ( std::alignment_of<T>::value > std::alignment_of<decltype( storage::local )>::value )>
			{
			};

			// construction of local values
			template <class ValueType, class T> struct local_construction
			{
				static void construct( ValueType &&_value, storage &_storage )
				{
					new ( &_storage.local ) T( std::forward<ValueType>( _value ) );
				}
			};
			// construction of heap values
			template <class ValueType, class T> struct heap_construction
			{
				static void construct( ValueType &&_value, storage &_storage )
				{
					_storage.heap = new T( std::forward<ValueType>( _value ) );
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

		// construct a value of type ValueType
		template <class ValueType> void				construct( ValueType &&_value );
		template <class ValueType> ValueType *		cast();
		template <class ValueType> const ValueType *cast() const;

		// the value storage
		internal::storage m_Storage;
		// the pointer to the method table
		internal::vtable_type *m_VTable = nullptr;

		// any_cast must be a friend
		template <class ValueType> friend const ValueType *any_cast( const any *_operand ) noexcept;
		template <class ValueType> friend ValueType *	  any_cast( any *_operand ) noexcept;
	};
}

#include "any.inl"
