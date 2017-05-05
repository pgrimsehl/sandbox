#include "stdafx.h"

#include <core/any.h>
#include <core/types.h>

#include <string>
#include <vector>

#include "any_user.h"

// ---------------------------------------------------------------------------
const core::any_type_info& any_user::use_any_id()
{
	//const core::any_type_info &info00 = core::any_typeid<std::string>();
	const core::any_type_info &info01 = core::any_typeid<int>();

	core::any_typeid<int>();
	core::any_typeid<u32>();
	core::any_typeid<u64>();
	core::any_typeid<u8>();

	return info01;
}

// ---------------------------------------------------------------------------
bool any_user::use_vector()
{
	std::vector<int> test;
	test.push_back( 25 );
	return true;
}
