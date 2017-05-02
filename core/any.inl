#include "stdafx.h"

#include "core/any.h"

namespace core
{
	// (N4562 6.3.1.1)
	constexpr any::any()
		: m_VTable( nullptr )
	{
	}

	// (N4562 6.3.1.3)
	any::any( const any &_rhs )
	{
		if ( !_rhs.empty() )
		{
			_rhs.m_VTable->copy( m_Storage, _rhs.m_Storage );
			m_VTable = _rhs.m_VTable;
		}
	}

	// (N4562 6.3.1.6)
	any::any( any &&_rhs )
	{
		if ( !_rhs.empty() )
		{
			_rhs.m_VTable->move( m_Storage, _rhs.m_Storage );
			_rhs.m_VTable = nullptr;
		}
	}

	// (N4562 6.3.1.11) must not participate in overloading if T is of type any
	template <class T, typename> any::any( T &&_value )
	{
		// TODO: T must be copy constructible
		construct( std::forward<T>( _value ) );
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

	template <typename T> any &any::operator=( T &&_rhs )
	{
		any( std::forward<ValueType>( _rhs ) ).swap( *this );
		return *this;
	}

	// (N4562 6.3.1.15)
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

	bool any::empty() const
	{
		return ( nullptr == m_VTable );
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
