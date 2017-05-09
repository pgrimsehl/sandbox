// playground.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
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
#include <string>
#include <tuple>
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

	size_t h0   = typeid( A ).hash_code();
	size_t h1   = typeid( A & ).hash_code();
	size_t h2   = typeid( A * ).hash_code();
	size_t h3   = typeid( A *const ).hash_code();
	size_t h0_c = typeid( A const ).hash_code();
	size_t h1_c = typeid( A const & ).hash_code();
	size_t h2_c = typeid( A const * ).hash_code();
	size_t h3_c = typeid( A const *const ).hash_code();

	size_t g0   = typeid( SubA ).hash_code();
	size_t g1   = typeid( SubA & ).hash_code();
	size_t g2   = typeid( SubA * ).hash_code();
	size_t g3   = typeid( SubA *const ).hash_code();
	size_t g0_c = typeid( SubA const ).hash_code();
	size_t g1_c = typeid( SubA const & ).hash_code();
	size_t g2_c = typeid( SubA const * ).hash_code();
	size_t g3_c = typeid( SubA const *const ).hash_code();

	size_t ch0   = core::type_id<A>().hash_code();
	size_t ch1   = core::type_id<A &>().hash_code();
	size_t ch2   = core::type_id<A *>().hash_code();
	size_t ch3   = core::type_id<A *const>().hash_code();
	size_t ch0_c = core::type_id<A const>().hash_code();
	size_t ch1_c = core::type_id<A const &>().hash_code();
	size_t ch2_c = core::type_id<A const *>().hash_code();
	size_t ch3_c = core::type_id<A const *const>().hash_code();

	size_t cg0   = core::type_id<SubA>().hash_code();
	size_t cg1   = core::type_id<SubA &>().hash_code();
	size_t cg2   = core::type_id<SubA *>().hash_code();
	size_t cg3   = core::type_id<SubA *const>().hash_code();
	size_t cg0_c = core::type_id<SubA const>().hash_code();
	size_t cg1_c = core::type_id<SubA const &>().hash_code();
	size_t cg2_c = core::type_id<SubA const *>().hash_code();
	size_t cg3_c = core::type_id<SubA const *const>().hash_code();

	int				 a0;
	int &			 a1 = a0;
	int *			 a2;
	int *const		 a3   = nullptr;
	int const		 a0_c = 0;
	int const &		 a1_c = a0;
	int const *		 a2_c;
	int const *const a3_c = nullptr;

	size_t ha0   = typeid( a0 ).hash_code();
	size_t ha1   = typeid( a1 ).hash_code();
	size_t ha2   = typeid( a2 ).hash_code();
	size_t ha3   = typeid( a3 ).hash_code();
	size_t ha0_c = typeid( a0_c ).hash_code();
	size_t ha1_c = typeid( a1_c ).hash_code();
	size_t ha2_c = typeid( a2_c ).hash_code();
	size_t ha3_c = typeid( a3_c ).hash_code();

	const int													test_int = 0;
	std::remove_reference_t<std::remove_cv_t<int>>				vh0;
	std::remove_reference_t<std::remove_cv_t<int &>>			vh1;
	std::remove_reference_t<std::remove_cv_t<int *>>			vh2;
	std::remove_reference_t<std::remove_cv_t<const int>>		vh0_c;
	std::remove_reference_t<std::remove_cv_t<int const &>>		vh1_c = test_int;
	std::remove_reference_t<std::remove_cv_t<const int *>>		vh2_c;
	std::remove_reference_t<std::remove_cv_t<const int *const>> vh2_cc;

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
	//x.emplace<std::string>({"doean not work!"});
	x.emplace<std::string>(std::string{ "works!" });
	x.emplace<std::string>("works!");

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
