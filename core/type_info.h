#pragma once

#include <memory> //std::addressof

namespace core
{
	// ---------------------------------------------------------------------------
	// The type info instance for each type (equivalent of std::type_info)
	// ---------------------------------------------------------------------------
	class type_info final
	{
		constexpr type_info() noexcept
		{
		}
		type_info( type_info && )						   = delete;
		type_info( const type_info & )					   = delete;
		type_info &		operator=( const type_info &_rhs ) = delete;
		template <class T> friend const type_info &type_id();
		template <class T> friend class type_info_wrapper;

	public:
		bool operator==( const type_info &_rhs ) const
		{
			return std::addressof( *this ) == std::addressof( _rhs );
		}

		bool operator!=( const type_info &_rhs ) const
		{
			return std::addressof( *this ) != std::addressof( _rhs );
		}

		bool before( const type_info &_rhs ) const
		{
			return std::addressof( *this ) < std::addressof( _rhs );
		}

		std::size_t hash_code() const
		{
			static_assert( sizeof( std::size_t ) >= sizeof( std::uintptr_t ), "pointer does not fit into std::size_t" );
			return reinterpret_cast<size_t>( std::addressof( *this ) );
		}

		const char *name() const
		{
			return "core::type_info does not supply type names";
		}
	};

	// ---------------------------------------------------------------------------
	// Internal wrapper to instantiate a distinct class for each type
	// ---------------------------------------------------------------------------
	template <class T> class type_info_wrapper final
	{
		type_info_wrapper()							   = delete;
		type_info_wrapper( type_info_wrapper && )	  = delete;
		type_info_wrapper( const type_info_wrapper & ) = delete;
		type_info_wrapper &operator=( const type_info_wrapper & ) = delete;

		static const type_info id;

		template <class T> friend const type_info &type_id();
	};
	template <class T> const type_info type_info_wrapper<T>::id;

	// ---------------------------------------------------------------------------
	// The function that returns the type_info instance for a given type (compile-time)
	// (equivalent of typeid operator)
	// To match the behavior of the typeid operator ValueType is transformed
	// using std::remove_reference<typename std::remove_cv<ValueType>::type>::type
	// ---------------------------------------------------------------------------
	template <class ValueType> const type_info &type_id()
	{
		return type_info_wrapper<
			typename std::remove_reference<typename std::remove_cv<ValueType>::type>::type>::id;
	};
}