#pragma once

using i8  = signed char;
using u8  = unsigned char;
using i16 = signed short int;
using u16 = unsigned short int;
using i32 = signed long int;
using u32 = unsigned long int;
using i64 = signed long long int;
using u64 = unsigned long long int;
using f32 = float;
using f64 = double;

class any_type_id
{
public:
	constexpr any_type_id()
	{
	}

	template <class T> friend const any_type_id &any_type_of();

public:
	bool operator==( const any_type_id &_rhs ) const
	{
		return this == &_rhs;
	}
};

template <class T> class any_type_wrapper
{
	static const any_type_id id;

	template <class T> friend const any_type_id &any_type_of();
};
template <class T> const any_type_id any_type_wrapper<T>::id;

template <class ValueType> const any_type_id &any_type_of()
{
	using T = typename std::decay<ValueType>::type;
	return any_type_wrapper<T>::id;
};

// template <class T> class type_of
//{
// public :
//	static u32 const * const  type_id()
//	{
//		return &id;
//	}
//
// private :
//	static const u32 id = 0;
//};
