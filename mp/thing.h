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

			template <typename L> struct get_id_list;

			template <template <class ...Ts> class L, class T, u32 LID> struct get_id_list< L<>;


		public:
			using property_list = mp::tl::TypeList<props...>;
		};
	}
}