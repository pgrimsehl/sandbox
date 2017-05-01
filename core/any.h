#pragma once

namespace core
{
	class any
	{
	public:
		constexpr any();
		any( const any &other );
		any( any &&other );
		template <class ValueType> any( ValueType &&value );

		any &							   operator=( const any &rhs );
		any &							   operator=( any &&rhs );
		template <typename ValueType> any &operator=( ValueType &&rhs );

		~any();

		void reset();
		void swap( any &other );

		bool			 has_value() const;
		const type_info &type() const;

	private:
		struct internal
		{
			struct storage;
			struct value_holder
			{
				virtual const type_info &type() const										= 0;
				virtual void			 move( const value_holder &, value_holder & ) const = 0;
				virtual void			 swap( value_holder &, value_holder & ) const		= 0;
				virtual void			 copy( const value_holder &, value_holder & ) const = 0;
				virtual void			 destroy( value_holder & ) const					= 0;
			};

			struct storage
			{
				union {
					unsigned char stack[ 8 ]; // TODO: proper alignment, size etc
					value_holder *heap;
				};
			};

			template <class ValueType> struct dynamic_holder : public value_holder
			{

				virtual const type_info &type() const override
				{
					return typeid( ValueType );
				}
				virtual void move( const storage &, storage & ) override
				{
				}
				virtual void swap( storage &, storage & ) const override
				{
				}
				virtual void copy( const storage &, storage & ) override
				{
				}
				virtual void destroy( storage & ) const override
				{
				}
			};
		};

		template <class ValueType> void construct( ValueType &&_value ){};

		internal::storage		storage;
		internal::value_holder *value = nullptr;
	};

	// template <class ValueType> ValueType		any_cast( const any &operand );
	// template <class ValueType> ValueType		any_cast( any &operand );
	// template <class ValueType> ValueType		any_cast( any &&operand );
	// template <class ValueType> const ValueType *any_cast( const any *operand );
	// template <class ValueType> ValueType *		any_cast( any *operand );
}

#include "any.inl"
