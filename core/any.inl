#include "stdafx.h"

#include "core/any.h"

namespace core
{
	constexpr any::any()
	{
	}

	any::any( const any &other )
	{
	}

	any::any( any &&other )
	{
	}

	template <class ValueType> any::any( ValueType &&value )
	{
	}

	any &any::operator=( const any &_rhs )
	{
		any( _rhs ).swap( *this );
		return *this;
	}

	any &any::operator=( any &&_rhs )
	{
		any( std::move( _rhs ) ).swap( *this );
		return *this;
	}

	template <typename ValueType> any &any::operator=( ValueType &&_rhs )
	{
		any( std::forward<ValueType>( _rhs ) ).swap( *this );
		return *this;
	}

	any::~any()
	{
		reset();
	}

	void any::reset()
	{
	}

	void any::swap( any &other )
	{
	}

	bool any::has_value() const
	{
		return ( nullptr != m_VTable );
	}

	const type_info &any::type() const
	{
		return typeid( void );
	}

	// template <class ValueType> ValueType		any_cast( const any &operand );
	// template <class ValueType> ValueType		any_cast( any &operand );
	// template <class ValueType> ValueType		any_cast( any &&operand );
	// template <class ValueType> const ValueType *any_cast( const any *operand );
	// template <class ValueType> ValueType *		any_cast( any *operand );
};
