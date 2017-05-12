#pragma once

#include "core.h"

#include <limits> // std::numeric_limits

namespace core
{
	//	-----------------------------------------------------------------------
	//	core::any_serializer_base
	//	Serialization support for core::any
	//	-----------------------------------------------------------------------
	//	Requirements for the Traits type
	//	-----------------------------------------------------------------------
	//	Defines the following static member functions:
	//		void storeTypeId( type_id_type, ostream_type & )
	//		type_id_type loadTypeId( istream_type & )
	//
	//	Defines the following types:
	//		istream_type:	
	//			type of stream to read from
	//
	//		ostream_type:	
	//			type of stream to write to
	//
	//		type_id_type:	
	//			integral type to be used as type index
	//
	//		type_serializer_type:
	//			a template class, taking exactly one template type parameter 
	//			ValueType to identify the type that is serialized
	//			type_serializer_type defines the following static member functions:
	//				void store(const core::any &_any, ostream_type &_out)
	//				void load(core::any &_any, istream_type &_in)
	//			_any is guaranteed to match _any.type() == core::type_id<ValueType>() when these
	//			functions are invoked 	
	//
	//	-----------------------------------------------------------------------
	template <class Traits, typename... Ts> struct any_serializer_base
	{
		// local redefinition of trait types
		using traits_type  = Traits;
		using ostream_type = typename traits_type::ostream_type;
		using istream_type = typename traits_type::istream_type;
		template <class ValueType>
		using type_serializer_type = typename traits_type::template type_serializer_type<ValueType>;
		using type_id_type		   = typename traits_type::type_id_type;

		// transform the supplied value types and make each type unique
		using raw_types		= mp::tl::typelist<Ts...>;
		using decayed_types = mp::fn::transform_t<raw_types, std::decay_t>;
		using unique_types  = mp::tl::unique_t<decayed_types>;
		using type_count	= mp::tl::list_size<unique_types>;

		static_assert( std::is_integral<type_id_type>::value, "type_id_type must be an integral type" );
		static_assert( std::numeric_limits<type_id_type>::max() >= type_count::value,
					   "type_id_type is too small to index all types" );

		// function template to access the type_info for each type
		template <typename ValueType> static const core::type_info &type_of()
		{
			return core::type_id<ValueType>();
		}

		// required function signatures
		using store_func_type = void ( * )( const any &, ostream_type & );
		using load_func_type  = void ( * )( any &, istream_type & );
		using type_func_type  = const ::core::type_info &(*)();

		// this wrapper class is used to have access to the types in unique_types
		template <class L> struct func_wrapper;
		template <template <typename...> class L, typename... Ts> struct func_wrapper<L<Ts...>>
		{
			constexpr func_wrapper()			 = delete;
			func_wrapper( const func_wrapper & ) = delete;
			func_wrapper( func_wrapper && )		 = delete;
			static store_func_type store_funcs[ type_count::value ];
			static load_func_type  load_funcs[ type_count::value ];
			static type_func_type  type_funcs[ type_count::value ];
		};

		using functions = func_wrapper<unique_types>;

		static void store( const any &_any, ostream_type &_out )
		{
			// linear search
			for ( type_id_type type_id = 0; static_cast<type_id_type>( type_count::value ) > type_id;
				  ++type_id )
			{
				if ( functions::type_funcs[ type_id ]() == _any.type() )
				{
					traits_type::storeTypeId( type_id, _out );
					functions::store_funcs[ type_id ]( _any, _out );
					return;
				}
			}
			// CORE_RAISE();
		}

		static void load( any &_any, istream_type &_in )
		{
			type_id_type type_id = traits_type::loadTypeId( _in );
			if ( type_count::value > type_id )
			{
				functions::load_funcs[ type_id ]( _any, _in );
				return;
			}
			// CORE_RAISE( );
		}
	};

	template <class Traits, typename... ValueTypes>
	template <template <typename...> class L, typename... Ts>
	typename any_serializer_base<Traits, ValueTypes...>::store_func_type
		any_serializer_base<Traits, ValueTypes...>::func_wrapper<
			L<Ts...>>::store_funcs[ any_serializer_base<Traits, ValueTypes...>::type_count::value ] = {
			any_serializer_base<Traits, ValueTypes...>::type_serializer_type<Ts>::store...
		};
	template <class Traits, typename... ValueTypes>
	template <template <typename...> class L, typename... Ts>
	typename any_serializer_base<Traits, ValueTypes...>::load_func_type
		any_serializer_base<Traits, ValueTypes...>::func_wrapper<
			L<Ts...>>::load_funcs[ any_serializer_base<Traits, ValueTypes...>::type_count::value ] = {
			any_serializer_base<Traits, ValueTypes...>::type_serializer_type<Ts>::load...
		};
	template <class Traits, typename... ValueTypes>
	template <template <typename...> class L, typename... Ts>
	typename any_serializer_base<Traits, ValueTypes...>::type_func_type
		any_serializer_base<Traits, ValueTypes...>::func_wrapper<
			L<Ts...>>::type_funcs[ any_serializer_base<Traits, ValueTypes...>::type_count::value ] = {
			&any_serializer_base<Traits, ValueTypes...>::type_of<Ts>...
		};
}