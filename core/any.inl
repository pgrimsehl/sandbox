#include "stdafx.h"

#include "core/any.h"

namespace core
{
	// (N4617 6.3.1-1)
	any::any() noexcept
		: m_VTable( nullptr )
	{
	}

	// (N4617 6.3.1-3)
	any::any( const any &_other )
	{
		if ( !_other.empty() )
		{
			_other.m_VTable->copy_construct( m_Storage, _other.m_Storage );
			m_VTable = _other.m_VTable;
		}
	}

	// (N4617 6.3.1-6)
	any::any( any &&_other ) noexcept
	{
		if ( !_other.empty() )
		{
			_other.m_VTable->move_construct( m_Storage, _other.m_Storage );
			_other.m_VTable = nullptr;
		}
	}

	// (N4617 6.3.1-13) must not participate in overloading if decay_t<ValueType> is of type any
	template <class ValueType, typename> any::any( ValueType &&_value )
	{
		// (N4617 6.3.1-11)
		static_assert( std::is_copy_constructible<typename std::decay<ValueType>::type>::value,
					   "(N4617 6.3.1.11): 'T shall satisfy the CopyConstructible requirements.'" );
		construct( std::forward<ValueType>( _value ) );
	}

	// (N4617 6.3.2-1)
	any &any::operator=( const any &_rhs )
	{
		any( _rhs ).swap( *this );
		return *this;
	}

	// (N4617 6.3.2-5)
	any &any::operator=( any &&_rhs ) noexcept
	{
		any( std::move( _rhs ) ).swap( *this );
		return *this;
	}

	// (N4617 6.3.2-9)
	template <typename ValueType> any &any::operator=( ValueType &&_rhs )
	{
		any( std::forward<ValueType>( _rhs ) ).swap( *this );
		return *this;
	}

	// (N4617 6.3.1-15)
	any::~any()
	{
		clear();
	}

	// (N4617 6.3.3-1)
	void any::clear() noexcept
	{
		if ( nullptr != m_VTable )
		{
			m_VTable->destroy( m_Storage );
			m_VTable = nullptr;
		}
	}

	// (N4617 6.3.3-4)
	void any::swap( any &_rhs ) noexcept
	{
		if ( this != &_rhs )
		{
			any other_copy( std::move( _rhs ) );
			if ( nullptr != m_VTable )
			{
				m_VTable->move_construct( _rhs.m_Storage, m_Storage );
				_rhs.m_VTable = m_VTable;
				m_VTable	  = nullptr;
			}
			if ( nullptr != other_copy.m_VTable )
			{
				other_copy.m_VTable->move_construct( m_Storage, other_copy.m_Storage );
				m_VTable			= other_copy.m_VTable;
				other_copy.m_VTable = nullptr;
			}
		}
	}

	// (N4617 6.3.4-1)
	bool any::empty() const noexcept
	{
		return ( nullptr == m_VTable );
	}

	// (N4617 6.3.4-3)
	const type_info &any::type() const noexcept
	{
		if ( !empty() )
		{
			return m_VTable->type();
		}
		return typeid( void );
	}

	// creates the internal representation of _value
	template <class ValueType> void any::construct( ValueType &&_value )
	{
		// for all further processing, we use the decay type T of ValueType
		using T = typename std::decay<ValueType>::type;
		// select heap or local storage
		using storage_type = typename std::conditional<internal::allocate_on_heap<T>::value, typename internal::heap_storage<T>,
													   typename internal::local_storage<T>>::type;
		// select heap or local construction
		using construct_type = typename std::conditional<internal::allocate_on_heap<T>::value, typename internal::heap_construction<ValueType, T>,
														 typename internal::local_construction<ValueType, T>>::type;

		// the static vtable for this type
		static internal::vtable_type vtable = { storage_type::type, storage_type::copy_construct, storage_type::move_construct, storage_type::swap,
												storage_type::destroy };
		// construct the value
		construct_type::construct( std::forward<ValueType>( _value ), m_Storage );
		// set the vtable pointer
		m_VTable = &vtable;
	};

	template <class ValueType> ValueType *any::cast()
	{
		using T			= typename std::decay<ValueType>::type;
		using cast_type = typename std::conditional<internal::allocate_on_heap<T>::value, typename internal::heap_cast<ValueType>,
													typename internal::local_cast<ValueType>>::type;
		return cast_type::cast( m_Storage );
	}
	template <class ValueType> const ValueType *any::cast() const
	{
		using T			= typename std::decay<ValueType>::type;
		using cast_type = typename std::conditional<internal::allocate_on_heap<T>::value, typename internal::heap_cast<ValueType>,
													typename internal::local_cast<ValueType>>::type;
		return cast_type::cast( m_Storage );
	}

	// N4618 20.8.2, class bad_any_cast
	class bad_any_cast : public std::bad_cast
	{
	public:
		const char *what() const noexcept override
		{
			return "bad any cast";
		}
	};

	// N4618 20.8.4, non-member functions
	void swap( any &_x, any &_y ) noexcept
	{
		_x.swap( _y );
	}

	// template <class T, class... Args> any		   make_any( Args &&... args );
	// template <class T, class U, class... Args> any make_any( initializer_list<U> il, Args &&... args );
	template <class ValueType> ValueType any_cast( const any &_operand )
	{
		static_assert( std::is_reference<ValueType>::value || std::is_copy_constructible<ValueType>::value,
					   "Requires: is_reference_v<ValueType> is true or is_copy_constructible_v<ValueType> is true." );
		auto result = any_cast<typename std::add_const<typename std::remove_reference<ValueType>::type>::type>( &_operand );
		if ( nullptr == result )
		{
			throw bad_any_cast();
		}
		return *result;
	}
	template <class ValueType> ValueType any_cast( any &_operand )
	{
		static_assert( std::is_reference<ValueType>::value || std::is_copy_constructible<ValueType>::value,
					   "Requires: is_reference_v<ValueType> is true or is_copy_constructible_v<ValueType> is true." );
		auto result = any_cast<typename std::remove_reference<ValueType>::type>( &_operand );
		if ( nullptr == result )
		{
			throw bad_any_cast();
		}
		return *result;
	}
	template <class ValueType> ValueType any_cast( any &&_operand )
	{
		static_assert( std::is_reference<ValueType>::value || std::is_copy_constructible<ValueType>::value,
					   "Requires: is_reference_v<ValueType> is true or is_copy_constructible_v<ValueType> is true." );
		auto result = any_cast<typename std::remove_reference<ValueType>::type>( &_operand );
		if ( nullptr == result )
		{
			throw bad_any_cast();
		}
		return *result;
	}
	template <class ValueType> const ValueType *any_cast( const any *_operand ) noexcept
	{
		if ( ( nullptr != _operand ) && ( _operand->type() == typeid( ValueType ) ) )
		{
			return _operand->cast<ValueType>();
		}
		return nullptr;
	}
	template <class ValueType> ValueType *any_cast( any *_operand ) noexcept
	{
		if ( ( nullptr != _operand ) && ( _operand->type() == typeid( ValueType ) ) )
		{
			return _operand->cast<ValueType>();
		}
		return nullptr;
	}
};
