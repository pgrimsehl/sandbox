// playground.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"

#include <mp/function.h>
#include <mp/thing.h>
#include <mp/variant.h>

#include <core/any.h>
#include <core/udl.h>

#include <string>
#include <tuple>

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
// template <class T> struct has_lala<T, std::is_same<decltype(std::declval<T>().lala()), decltype(std::declval<T>().lala())>::value> : public
// std::true_type {}; template <class T> struct has_lala<T, std::is_same<T,T>> : public std::true_type {};

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

class MyThing : public mp::gen::thing<mp::gen::prop<"Name"_crc32, std::string>, mp::gen::prop<"PosX"_crc32, f32>, mp::gen::prop<"PosY"_crc32, f32>,
									  mp::gen::prop<"Width"_crc32, u16>, mp::gen::prop<"Height"_crc32, u32>>
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

int main()
{
	u32 udl0 = "CRC32 UDL"_crc32;

	SubA suba;

	suba.myMethod();
	suba.A::myMethod();
	std::string val_suba = suba.myValue;
	u8			val_a	= suba.A::myValue;

	MyThing thingy;

	f32 x					   = thingy.get<"PosX"_crc32>();
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
	// For C++11 (http://stackoverflow.com/questions/13002368/template-constructor-in-a-class-template-how-to-explicitly-specify-template-ar)
	// You cannot use T for the class template parameter and the constructor template parameter. But, to answer your question, from[14.5.2p5] :
	// Because the explicit template argument list follows the function template name, and because conversion member function templates
	// and constructor member function templates are called without using a function name, there is no way to provide an explicit template
	// argument list for these function templates.
	// In C++17 std::in_place_type_t<T> is used for these constructors
	// MyVariant v1<std::string, const char*>("dumdidum");

	return 0;
}
