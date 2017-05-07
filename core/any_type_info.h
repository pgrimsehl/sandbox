#pragma once

namespace core
{
	// ---------------------------------------------------------------------------
	// Simple type checking mechanism when no rtti is present and
	// usage of typeid operator is not possible.
	// NOTE: This is not meant to be a replacement of type_info / typeid
	// and is only used in core::any to allow type identity checking
	// when no rtti is available.
	// ---------------------------------------------------------------------------

	// ---------------------------------------------------------------------------
	// The type info instance for each type (equivalent of std::type_info)
	// ---------------------------------------------------------------------------
	class any_type_info final
	{
		constexpr any_type_info()
		{
		}

		template <class T> friend const any_type_info &any_typeid();
		template <class T> friend class any_type_wrapper;

	public:
		bool operator==( const any_type_info &_rhs ) const
		{
			return this == &_rhs;
		}
		bool operator!=( const any_type_info &_rhs ) const
		{
			return this != &_rhs;
		}
	};

	// ---------------------------------------------------------------------------
	// Internal wrapper to instantiate a distinct class for each type
	// ---------------------------------------------------------------------------
	template <class T> class any_type_wrapper final
	{
		any_type_wrapper() = delete;

		static const any_type_info id;

		template <class T> friend const any_type_info &any_typeid();
	};
	template <class T> const any_type_info any_type_wrapper<T>::id;

	// ---------------------------------------------------------------------------
	// The function that returns the any_type_info for a given type (compile-time)
	// (equivalent of typeid operator)
	// To match the behaviour of the typeid operator ValueType is tranformed
	// using std::remove_reference<typename std::remove_cv<ValueType>::type>::type
	// ---------------------------------------------------------------------------
	template <class ValueType> const any_type_info &any_typeid()
	{
		return any_type_wrapper<
			typename std::remove_reference<typename std::remove_cv<ValueType>::type>::type>::id;
	};
}