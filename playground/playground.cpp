// playground.cpp : Definiert den Einstiegspunkt f�r die Konsolenanwendung.
//

#include "stdafx.h"

#include <mp/function.h>
#include <mp/thing.h>
#include <mp/variant.h>

#include <core/any.h>
#include <core/type_info.h>
#include <core/udl.h>

#include <test_lib/any_user.h>

#include <cassert>
#include <istream>
#include <ostream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

class A
{
public:
	int myMethod()
	{
		return 0;
	};
	u8 myValue = 0;
};

class B
{
};

class SubA : public A
{
public:
	int myMethod()
	{
		return 1;
	};
	std::string myValue;
};

class SubB : public B
{
};

class SubSubA : public SubA
{
};

void call_void( ... )
{
}

using MyVariant = mp::variant<i32, u32, bool, std::string>;

// std::is_copy_constructible<MyVariant>;

// template <class T, typename U > struct has_lala;
// template <class T> struct has_lala<T, void> : public std::false_type {};
// template <class T> struct has_lala<T, std::is_same<decltype(std::declval<T>().lala()),
// decltype(std::declval<T>().lala())>::value> : public std::true_type {}; template <class T> struct
// has_lala<T, std::is_same<T,T>> : public std::true_type {};

class HasLala
{
public:
	void lala();
};

class DoesntHaveLala
{
public:
	void lulu();
};

// auto lala_test0 = has_lala<HasLala>::value;
// auto lala_test1 = has_lala<DoesntHaveLala>::value;

using typelist_pair = mp::tl::typelist<char, short>;
using typelist0		= mp::tl::typelist<char, short, int>;
using typelist1		= mp::tl::push_back_t<typelist0>;

typelist0 list0;
typelist1 list1;
using typelist2 = mp::tl::concat_t<typelist0, typelist1>;
typelist2 list2;
using typelist3 = mp::tl::erase_t<typelist2, int>;
typelist3 list3;
using typelist4 = mp::tl::erase_t<typelist2, void *>;
typelist4 list4;
using typelist5 = mp::tl::unique_t<typelist2>;
typelist5 list5;
using typelist6 = mp::tl::push_back_t<typelist0, A, B, SubA, SubB>;
typelist6 list6;

// using type0 = mp::tl::type_at_t<typelist0, 2>;
// type0 t0;
// using type1 = mp::fn::transform_t<typelist6, std::tuple>;
// type1 t1;
// using type2 = mp::fn::rename_t<typelist_pair, std::pair>;
// type2 t2;
// using type3 = mp::fn::rename_t<typelist6, std::tuple>;
// type3 t3;
// using type4 = std::conditional_t<mp::tl::contains_v<typelist0, int>, A, B>;
// type4 t4;

class MyThing
	: public mp::gen::thing<mp::gen::prop<"Name"_crc32, std::string>, mp::gen::prop<"PosX"_crc32, f32>,
							mp::gen::prop<"PosY"_crc32, f32>, mp::gen::prop<"Width"_crc32, u16>,
							mp::gen::prop<"Height"_crc32, u32>>
{
public:
	MyThing() = default;
};

mp::tl::front_t<typelist0>									  front_type;
mp::tl::back_t<typelist0>									  back_type;
mp::tl::pop_front_t<typelist0>								  pop_front_type;
mp::tl::pop_back_t<typelist0>								  pop_back_type;
mp::tl::inverse_t<typelist6>								  typelist6_inverse;
mp::tl::replace_at_t<typelist6, 5, MyThing>					  typelist6_replaced5;
mp::tl::replace_t<typelist2, char, MyThing>					  typelist2_replaced;
mp::tl::replace_t<decltype( typelist2_replaced ), short, i64> typelist2_replaced_replaced;
using erased_type = mp::tl::type_at_t<typelist2, 2>;
mp::tl::erase_at_t<typelist2, 2>									typelist2_erased_2;
mp::tl::insert_at_t<decltype( typelist2_erased_2 ), 2, erased_type> typelist2_restored;
mp::tl::swap_t<typelist2, 2, 3>										typelist2_swap;

template <class T, class U> struct sort_by_size
{
	using type = typename std::conditional<( sizeof( T ) < sizeof( U ) ), T, U>::type;
};
mp::tl::sort_t<typelist2, sort_by_size>		 sorted;
mp::tl::split_at_t0<typelist2, 4>			 splitted_left;
mp::tl::split_at_t1<typelist2, 4>			 splitted_right;
mp::tl::replace_first_t<typelist2, int, u64> replaced_first;

core::any any_empty();
core::any any_int( 25 );
core::any any_cc_int( any_int );
core::any any_string( std::string( "upps" ) );

// std::string str = core::any_cast<const std::string &>( any_string );
// core::any_cast<std::string&>( any_string ) = "ladida";

std::decay_t<char>		   d0;
std::decay_t<const char>   d1;
std::decay_t<char *>	   d2;
std::decay_t<const char *> d3;
std::decay_t<char &>	   d4;
std::decay_t<const char &> d5;

std::remove_reference_t<char>		  r0;
std::remove_reference_t<const char>   r1 = 0;
std::remove_reference_t<char *>		  r2;
std::remove_reference_t<const char *> r3;
std::remove_reference_t<char &>		  r4;
std::remove_reference_t<const char &> r5 = r1;

namespace core
{
	template <typename ValueType, typename OStream, typename IStream> struct serializer final
	{
		serializer()					 = delete;
		serializer( const serializer & ) = delete;
		serializer( serializer && )		 = delete;

		template <typename... Args> void load( ValueType &_value, IStream, Args &&... );
		template <typename... Args> void store( const ValueType &_value, OStream, Args &&...... ) const;
	};
}

// specialize for std::streams
template <typename ValueType> struct core::serializer<ValueType, std::ostream, std::istream> final
{
	serializer()					 = default;
	serializer( const serializer & ) = delete;
	serializer( serializer && )		 = delete;

	using value_type   = ValueType;
	using ostream_type = std::ostream;
	using istream_type = std::ostream;

	void load( ValueType &_value, istream_type &_in )
	{
		_in >> _value;
	};
	void store( const ValueType &_value, ostream_type &_out ) const
	{
		_out << _value;
	};
};

namespace core
{
	using IArchive = std::istream;
	using OArchive = std::ostream;
};

using store_func = void ( * )( const core::any &_any, core::OArchive &_ar );
using load_func  = void ( * )( core::any &_any, core::IArchive &_ar );

template <typename ValueType> struct type_serializer
{
	static void store( const core::any &_any, core::OArchive &_ar )
	{
		_ar << core::any_cast<const ValueType &>( _any );
	}
	static void load( core::any &_any, core::IArchive &_ar )
	{
		_ar >> core::any_cast<ValueType &>( _any );
	}
};

template <class L> struct function_mgr;
template <template <typename...> class L, typename... Ts> struct function_mgr<L<Ts...>>
{
	constexpr function_mgr()
	{
	}
	using type_count = mp::tl::list_size<L<Ts...>>;
	static store_func store_funcs[ type_count::value ];
	static load_func  load_funcs[ type_count::value ];
};

template <template <typename...> class L, typename... Ts>
store_func function_mgr<L<Ts...>>::store_funcs[ function_mgr<L<Ts...>>::type_count::value ] = {
	type_serializer<Ts>::store...
};
template <template <typename...> class L, typename... Ts>
load_func function_mgr<L<Ts...>>::load_funcs[function_mgr<L<Ts...>>::type_count::value] = {
	type_serializer<Ts>::load...
};

using test_list = mp::tl::typelist<char, int, float>;

function_mgr<test_list> test_mgr;

// template <typename... Ts> struct any_serializer_base
//{
//	using raw_types		= mp::tl::typelist<Ts...>;
//	using decayed_types = mp::fn::transform_t<raw_types, std::decay_t>;
//	using unique_types  = mp::tl::unique_t<decayed_types>;
//	using unique_count  = mp::tl::list_size<unique_types>;
//	using size_type		= std::size_t;
//
//	using store_func = void ( * )( const core::any &_any, core::OArchive &_ar );
//	using load_func  = void ( * )( core::any &_any, const core::OArchive &_ar );
//
//	template <typename ValueType> struct type_serializer
//	{
//		static void store( const core::any &_any, core::OArchive &_ar )
//		{
//			_ar << any_cast<const ValueType &>( _any );
//		}
//		static void load( core::any &_any, const core::OArchive &_ar )
//		{
//			_ar >> any_cast<ValueType &>( _any );
//		}
//	};
//
//	template <class L> struct function_mgr;
//	template <template <typename...> class L, typename... Ts> struct function_mgr<L<Ts...>>
//	{
//		constexpr function_mgr()
//		{
//		}
//
//		using type_count = mp::tl::list_size<L<Ts...>>;
//		static store_func store_funcs[ type_count::value ];
//		static load_func  load_funcs[ type_count::value ];
//	};
//
//	using functions = function_mgr<unique_types>;
//};

// template <typename... Ts>
// template <template <typename...> class L, typename... Us>
// typename any_serializer_base<Ts...>::template function_mgr<L<Us...>>::store_func any_serializer_base<
//	Ts...>::template function_mgr<L<Us...>>::store_funcs[ any_serializer_base<Ts...>::template
// function_mgr<L<Us...>>::type_count::value ] = {
//	&any_serializer_base<Ts...>::type_serializer<Us>::store...
//};

// template <typename... Ts>
// template <template <typename...> class L, typename ...Us>
// typename any_serializer_base<Ts...>::template function_mgr<L<Us...>>::load_func any_serializer_base<
//	Ts...>::template function_mgr<L<Us...>>::load_funcs[any_serializer_base<Ts...>::unique_count::value]
//= { 	&any_serializer_base<Ts...>::type_serializer<Us>::load...
//};

// template <typename... Ts>
// template <typename... Us>
// typename any_serializer_base<Ts...>::function_mgr<Us...>::load_func = {
//	any_serializer_base<Ts...>::type_serializer<Us>::load...
//};

template <typename... Ts> struct any_serializer_base
{
	using raw_types		= mp::tl::typelist<Ts...>;
	using decayed_types = mp::fn::transform_t<raw_types, std::decay_t>;
	using unique_types  = mp::tl::unique_t<decayed_types>;
	using unique_count  = mp::tl::list_size<unique_types>;
	using size_type		= std::size_t;
	using functions		= function_mgr<unique_types>;
};

struct any_serializer : public any_serializer_base<i8, u8, i16, u16, i32, u32, i64, u64, f32, f64>
{
};

any_serializer as;

// any_serializer::store_funcs[ 0 ]

void any_store( const core::any &_any, core::OArchive &_ar )
{
	any_serializer::functions::store_funcs[ 0 ]( _any, _ar );
}

void any_load( core::any &_any, core::IArchive &_ar )
{
	any_serializer::functions::load_funcs[ 0 ]( _any, _ar );
}

int main()
{
	using namespace core;

	any x( 5 );						   // x holds int
	assert( any_cast<int>( x ) == 5 ); // cast to value
	any_cast<int &>( x ) = 10;		   // cast to reference
	assert( any_cast<int>( x ) == 10 );

	x = "Meow"; // x holds const char*
	assert( strcmp( core::any_cast<const char *>( x ), "Meow" ) == 0 );
	core::any_cast<const char *&>( x ) = "Harry";
	assert( strcmp( core::any_cast<const char *>( x ), "Harry" ) == 0 );

	x = std::string( "Meow" ); // x holds string
	std::string s, s2( "Jane" );
	s = std::move( any_cast<std::string &>( x ) ); // move from any
	assert( s == "Meow" );
	any_cast<std::string &>( x ) = move( s2 ); // move to any
	assert( any_cast<const std::string &>( x ) == "Jane" );
	std::string cat( "Meow" );

	const any y( cat ); // const y holds string
	assert( any_cast<const std::string &>( y ) == cat );
	// any_cast<std::string &>( y ); // error; cannot any_cast away const
	x.emplace<std::string>( "lala" );
	assert( any_cast<const std::string &>( x ) == "lala" );
	x.emplace<std::vector<float>>( { 0.0f, 1.0f } );
	// x.emplace<std::string>({"doean not work!"});
	x.emplace<std::string>( std::string{ "works!" } );
	x.emplace<std::string>( "works!" );

	x = core::make_any<std::vector<int>>( { 5, 4, 3, 2, 1 } );
	assert( any_cast<const std::vector<int> &>( x )[ 2 ] == 3 );

	core::any z( core::in_place_type<std::vector<int>>, { 1, 2, 3 } );

	u32 udl0 = "CRC32 UDL"_crc32;

	SubA suba;

	suba.myMethod();
	suba.A::myMethod();
	std::string val_suba = suba.myValue;
	u8			val_a	= suba.A::myValue;

	MyThing thingy;

	f32 tx					   = thingy.get<"PosX"_crc32>();
	u32 height				   = thingy.get<"Height"_crc32>();
	thingy.get<"Name"_crc32>() = "TheThing";

	bool a = mp::fn::is_baseclass_of<A, B>::value;
	bool b = mp::fn::is_baseclass_of<A, SubA>::value;
	bool c = mp::fn::is_baseclass_of<A, SubSubA>::value;
	bool d = mp::fn::is_subclass_of<A, SubA>::value;
	bool e = mp::fn::is_subclass_of<SubSubA, SubA>::value;
	bool f = mp::fn::is_subclass_of<void, SubA>::value;
	bool g = mp::fn::is_subclass_of<SubA, void>::value;
	bool h = mp::fn::is_baseclass_of<void, SubA>::value;
	bool i = mp::fn::is_baseclass_of<SubA, void>::value;

	mp::fn::strip_t<const char *const> k;
	call_void( c );
	call_void( d );

	MyVariant v0( 25ul );
	v0 = std::string( "lala" );
	v0.emplace<std::string>( "lala" );
	// For C++11
	// (http://stackoverflow.com/questions/13002368/template-constructor-in-a-class-template-how-to-explicitly-specify-template-ar)
	// You cannot use T for the class template parameter and the constructor template parameter. But, to
	// answer your question, from[14.5.2p5] : Because the explicit template argument list follows the
	// function template name, and because conversion member function templates and constructor member
	// function templates are called without using a function name, there is no way to provide an
	// explicit template argument list for these function templates. In C++17 std::in_place_type_t<T> is
	// used for these constructors MyVariant v1<std::string, const char*>("dumdidum");

	const core::type_info &u32_id	 = core::type_id<u32>();
	const core::type_info &u32_ptr_id = core::type_id<u32 *>();
	const core::type_info &string_id  = core::type_id<std::string>();

	bool same_types = core::type_id<std::string &>() == string_id;
	same_types		= core::type_id<int>() == string_id;

	std::vector<int> vec;
	vec.push_back( 12 );

	any_user::use_vector();

	if ( core::type_id<u32>() == any_user::use_any_id() )
	{
		return 1;
	}

	return 0;
}
