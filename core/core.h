#pragma once

#ifdef CORE_NO_EXCEPTIONS

#define CORE_TRY_BEGIN		{ if (1) {
#define CORE_CATCH(x)		} else if (0) {
#define CORE_CATCH_ALL		} else if (0) {
#define CORE_CATCH_END		} }
#define CORE_RAISE(x)
#define CORE_RERAISE
#define CORE_NOTHROW
#define CORE_NOTHROW_IF(x)
#define CORE_THROW(x, y)

#else /* CORE_USE_EXCEPTIONS */

#define CORE_TRY_BEGIN		try {
#define CORE_CATCH(x)		} catch (x) {
#define CORE_CATCH_ALL		} catch (...) {
#define CORE_CATCH_END		}
#define CORE_RAISE(x)		throw x
#define CORE_RERAISE		throw
#define CORE_NOTHROW		noexcept
#define CORE_NOTHROW_IF(x)	noexcept(x)
#define CORE_THROW(x, y)	throw x(y)

#endif /* CORE_USE_EXCEPTIONS */

#include "type_info.h"
