#include "stdafx.h"

#include "core/any.h"

namespace core
{
	// (N4617 6.3.1.1)
	any::any() noexcept
		: m_VTable( nullptr )
	{
	}

	// (N4617 6.3.1.3)
	any::any( const any &_other )
	{
		if ( !_other.empty() )
		{
			_other.m_VTable->copy( m_Storage, _other.m_Storage );
			m_VTable = _other.m_VTable;
		}
	}

	// (N4617 6.3.1.6)
	any::any( any &&_other ) noexcept
	{
		if ( !_other.empty() )
		{
			_other.m_VTable->move( m_Storage, _other.m_Storage );
			_other.m_VTable = nullptr;
		}
	}

	// (N4617 6.3.1.13) must not participate in overloading if decay_t<ValueType> is of type any
	template <class ValueType, typename> any::any( ValueType &&_value )
	{
		// (N4617 6.3.1.11)
		static_assert( std::is_copy_constructible<typename std::decay<ValueType>::type>::value,
					   "(N4617 6.3.1.11): 'T shall satisfy the CopyConstructible requirements.'" );
		construct( std::forward<ValueType>( _value ) );
	}

	// (N4617 6.3.2.1)
	any &any::operator=( const any &_rhs )
	{
		any( _rhs ).swap( *this );
		return *this;
	}

	// (N4617 6.3.2.5)
	any &any::operator=( any &&_rhs ) noexcept
	{
		any( std::move( _rhs ) ).swap( *this );
		return *this;
	}

	// (N4617 6.3.2.9)
	template <typename ValueType> any &any::operator=( ValueType &&_rhs )
	{
		any( std::forward<ValueType>( _rhs ) ).swap( *this );
		return *this;
	}

	// (N4617 6.3.1.15)
	any::~any()
	{
		clear();
	}

	// (N4617 6.3.3.1)
	void any::clear() noexcept
	{
		if ( nullptr != m_VTable )
		{
			m_VTable->destroy( m_Storage );
			m_VTable = nullptr;
		}
	}

	// (N4617 6.3.3.4)
	void any::swap( any &_rhs ) noexcept
	{
		if ( this != &_rhs )
		{
			any other_copy( _rhs );
			_rhs.clear();
			if ( nullptr != m_VTable )
			{
				m_VTable->move( _rhs.m_Storage, m_Storage );
				_rhs.m_VTable = m_VTable;
				m_VTable	  = nullptr;
			}
			if ( nullptr != other_copy.m_VTable )
			{
				other_copy.m_VTable->move( m_Storage, other_copy.m_Storage );
				m_VTable			= other_copy.m_VTable;
				other_copy.m_VTable = nullptr;
			}
		}
	}

	// (N4617 6.3.4.1)
	bool any::empty() const noexcept
	{
		return ( nullptr == m_VTable );
	}

	// (N4617 6.3.4.3)
	const type_info &any::type() const noexcept
	{
		return typeid( void );
	}

	// template <class ValueType> ValueType		any_cast( const any &operand );
	// template <class ValueType> ValueType		any_cast( any &operand );
	// template <class ValueType> ValueType		any_cast( any &&operand );
	// template <class ValueType> const ValueType *any_cast( const any *operand );
	// template <class ValueType> ValueType *		any_cast( any *operand );
};
