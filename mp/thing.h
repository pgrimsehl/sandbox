#pragma once

#include <type_traits>
#include <typeinfo>

#include <core/types.h>

#include "mp/typelist.h"

namespace mp
{
	namespace gen
	{
		template <u32 ID, class T> class prop
		{
		public:
			using value_type = T;
			using id_type	= std::integral_constant<u32, ID>;
			value_type value;
		};

		template <class... props> class thing : public props...
		{

		public:

			using property_list = mp::tl::TypeList<props...>;

			template <class L, u32 ID> struct get_type_of_id;
			template <template <class...> class L, class ...Ts, u32 ID> struct get_type_of_id<L<Ts...>, ID>
			{
				using head = tl::type_at_t<L<Ts...>, static_cast<size_t>(0)>;
				using type = std::conditional_t< std::is_same_v<std::integral_constant<u32, ID>, typename head::id_type>, typename head::value_type, typename get_type_of_id<tl::erase_t<L<Ts...>, head>, ID>::type>;
			};


			template <u32 PropID>
			typename get_type_of_id<property_list, PropID>::type get() const
			{
				return typename get_type_of_id<property_list, PropID>::type::value;
			}
		
		};
	}
}