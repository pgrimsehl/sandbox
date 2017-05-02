#pragma once

#include <type_traits> // std::aligned_union

namespace core
{
	class any
	{
	public:
		constexpr any();
		any( const any &other );
		any( any &&other );
		template <class ValueType> any( ValueType &&value );

		any &							   operator=( const any &rhs );
		any &							   operator=( any &&rhs );
		template <typename ValueType> any &operator=( ValueType &&rhs );

		~any();

		void reset();
		void swap( any &other );

		bool			 has_value() const;
		const type_info &type() const;

	private:
		struct internal
		{
			// Ok, this is awkward: swap and move for heap allocated value_holders will effectively just exchange the contents
			// of the any::value member that points to the heap allocated object. Additionally this operation is independent of the actual
			// type specialization of value_holer. Yet both methods are defined as member functions
			// of the value_holder object. We actually have to pass a reference to any::value to the swap and move
			// methods to be able to do this pointer exchange from within the value_holder class. This just does not feel right.
			// Of course we could remove these methods from the value_holder interface, but as soon as we want to optimize for small types
			// using stack space inside the any object and inplace-constructing value_holder objects there, swap and move will
			// become type dependent operations that need to be implemented in the value_holder class.
			//
			// A better way would be to have a table of static member functions, operating on a storage object, that contains
			// either a pointer or a local space. The pointers to these functions must be stored inside a lookup table and a pointer to this table
			// must be kept (manually managed vtable) when the concrete type is instantiated.

			// struct value_holder
			//{
			//	virtual const type_info &type() const													 = 0;
			//	virtual void			 move( value_holder *&_thisLoc, value_holder *&_rhsLoc ) const   = 0;
			//	virtual void			 swap( value_holder *&_thisLoc, value_holder *&_rhsLoc ) const   = 0;
			//	virtual void			 copy( value_holder *&_thisLoc, const value_holder &_rhs ) const = 0;
			//	virtual void			 destroy() const												 = 0;
			//};

			// template <class ValueType> struct value_holder_heap : public value_holder
			//{

			//	virtual const type_info &type() const override
			//	{
			//		return typeid( ValueType );
			//	}
			//	virtual void move( value_holder *&_thisLoc, value_holder *&_rhsLoc ) override
			//	{
			//		std::swap( _thisLoc, _rhsLoc );
			//		_rhsLoc = nullptr;
			//		// delete _rhsLoc;
			//	}
			//	virtual void swap( value_holder *&_thisLoc, value_holder *&_rhsLoc ) const override
			//	{
			//		std::swap( _thisLoc, _rhsLoc );
			//	}
			//	virtual void copy( value_holder *&_thisLoc, const value_holder &_rhs ) override
			//	{
			//		//_thisLoc = new
			//	}
			//	virtual void destroy() const override
			//	{
			//		delete this;
			//	}
			//};

			// sing storage = typename std::aligned_union<void &[], stack_holder<u64>>::type m_Storage;

			// the internal storage is a union to a pointer to a heap allocated object
			// and a local variable that holds a certain amount of space tho allocate
			// small objects
			struct storage
			{
				union {
					void *												   heap;
					std::aligned_union<sizeof( void * ), void * [2]>::type local;
				};
			};

			// this is the function table definition that will hold the method pointers
			struct vtable
			{
				const type_info &( *type )();
				void ( *move )( storage &_dst, storage &_src );
				void ( *swap )( storage &_dst, storage &_src );
				void ( *copy )( storage &_dst, const storage &_src );
				void ( *destroy )( storage &_dst );
			};

			// this is the type-dependent method set for heap allocated values
			template <class T> struct heap_value
			{
				static const type_info &type()
				{
					return typeid( T );
				}
				// move the the heap pointer
				static void move( storage &_dst, storage &_src )
				{
					_dst.heap = _src.heap;
					_src.heap = nullptr;
				}
				// exchange the heap pointers
				static void swap( storage &_dst, storage &_src )
				{
					std::swap( _dst.heap, _src.heap );
				}
				// construct a new object using T's copy constructor
				static void copy( storage &_dst, const storage &_src )
				{
					_dst.heap = new T( *reinterpret_cast<const T *>( _src.heap ) );
				}
				static void destroy( storage &_dst )
				{
					delete reinterpret_cast<T *>( _dst.heap );
				}
			};

			// helper template to determine if a value should be allocated on the heap
			// TODO: returns true in every case
			template <class T>
			struct allocate_on_heap : public std::true_type
			{
			};
		};


		template <class ValueType> void construct( ValueType &&_value ){};

		// the value storage
		internal::storage m_Storage;
		// the pointer to the method table
		internal::vtable *m_VTable = nullptr;
	};

	// template <class ValueType> ValueType		any_cast( const any &operand );
	// template <class ValueType> ValueType		any_cast( any &operand );
	// template <class ValueType> ValueType		any_cast( any &&operand );
	// template <class ValueType> const ValueType *any_cast( const any *operand );
	// template <class ValueType> ValueType *		any_cast( any *operand );
}

#include "any.inl"
