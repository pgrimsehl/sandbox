#include "stdafx.h"

#include <core/any.h>

#include <string>
#include <vector>

#include "any_user.h"

// ---------------------------------------------------------------------------
bool any_user::use_any_id()
{
	const core::any_type_info &info00 = core::any_typeid<std::string>();
	const core::any_type_info &info01 = core::any_typeid<int>();

	if ( info00 == info01 )
	{
		return true;
	}
	return false;
}

// ---------------------------------------------------------------------------
bool any_user::use_vector()
{
	std::vector<int> test;
	test.push_back( 25 );
	return true;
}
