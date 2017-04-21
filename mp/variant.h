#pragma once

#include <core/types.h>

#include <type_traits>
#include "mp/typelist.h"

namespace mp
{
	template <class... Ts> class variant
	{
	public:
		variant();
		variant( const variant &_rhs );
		template <class T> variant( T &&_t );
		// template <class T, class... Args> variant( Args&&... args ); // not possible in C++11
		~variant();

		variant &operator=(const variant &_rhs);

		template <class T> void set( const T &_value );
		template <class T> bool get( T &_value ) const;

		template <class T, class... Args> void emplace( Args&&... args );

	protected:
		template <class... Types> struct ValueHolderType
		{
			typename std::aligned_union<8, Types...>::type m_Storage;
			u8											   m_TypeIndex;
		};

		using TypeCount   = typename std::integral_constant<size_t, sizeof...( Ts )>;
		using MyTypes	 = mp::tl::TypeList<Ts...>;
		using ValueHolder = ValueHolderType<Ts...>;

		template <class T> struct TypeHelper
		{
			static T &value( ValueHolder &_holder )
			{
				return *reinterpret_cast<T *>( &_holder.m_Storage );
			}
			static const T &const_value( const ValueHolder &_holder )
			{
				return *reinterpret_cast<const T *>( &_holder.m_Storage );
			}
			static void default_construct( ValueHolder &_holder )
			{
				::new ( static_cast<void *>( &_holder.m_Storage ) ) T();
			}
			static void copy_construct( ValueHolder &_holder, const void *_value )
			{
				::new ( static_cast<void *>( &_holder.m_Storage ) ) T( *reinterpret_cast<const T *>( _value ) );
			}
			static void destroy( ValueHolder &_holder )
			{
				reinterpret_cast<T *>( &_holder.m_Storage )->~T();
			}
			static void assign( ValueHolder &_holder, const void *_value )
			{
				value( _holder ) = *reinterpret_cast<const T *>( _value );
			}
		};

		using default_construct_func = void ( * )( ValueHolder &_holder );
		using copy_construct_func	= void ( * )( ValueHolder &_holder, const void *_value );
		using destroy_func			 = void ( * )( ValueHolder &_holder );
		using assign_func			 = void ( * )( ValueHolder &_holder, const void *_value );
		static default_construct_func default_constructors[ TypeCount::value ];
		static copy_construct_func	copy_constructors[ TypeCount::value ];
		static destroy_func			  destructors[ TypeCount::value ];
		static assign_func			  assigners[ TypeCount::value ];

		ValueHolder m_Value;
	};
#include "variant.inl"
}