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

			using property_list = mp::tl::typelist<props...>;

			template <class L, u32 ID> struct get_type_of_id;
			template <template <class...> class L, class ...Ts, template <u32 TID, class> class T, class U, u32 ID> struct get_type_of_id<L<T<ID,U>,Ts...>, ID>
			{
				using type = T<ID,U>;
			};
			template <template <class...> class L, class ...Ts, u32 ID> struct get_type_of_id<L<Ts...>, ID>
			{
				using head = tl::type_at_t<L<Ts...>, static_cast<size_t>(0)>;
				using type = typename get_type_of_id<tl::erase_t<L<Ts...>, head>, ID>::type;
			};

			template <u32 PropID> using prop_t = typename get_type_of_id<property_list, PropID>::type;
			template <u32 PropID> using prop_value_t = typename prop_t<PropID>::value_type;

			template <u32 PropID>
			prop_value_t<PropID>& get()
			{
				return prop_t<PropID>::value;
			}
		
			template <u32 PropID>
			const prop_value_t<PropID>& get() const
			{
				return prop_t<PropID>::value;
			}
		};
	}
}