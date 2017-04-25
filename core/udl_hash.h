#pragma once

#include "core/types.h"

namespace udl_hash_details
{
	static constexpr u32 HASH_TABLE_PRIM = 4294900037UL;

	constexpr u32 calc_hash( const char *_string, size_t _size, u32 _parentHash )
	{
		return ( 0 == _size ) ? _parentHash : calc_hash( _string + 1, _size - 1, ( 113 * _parentHash + *_string ) % HASH_TABLE_PRIM );
	}
}

static constexpr u32 operator"" _hash( const char *_str, const size_t _size )
{
	return udl_hash_details::calc_hash( _str, _size, 0 );
}