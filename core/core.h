#pragma once

#if CORE_NO_EXCEPTIONS

#define CORE_TRY_BEGIN		{ if (1) {
#define CORE_CATCH(x)		} else if (0) {
#define CORE_CATCH_ALL		} else if (0) {
#define CORE_CATCH_END		} }
#define CORE_RAISE(x)
#define CORE_RERAISE
#define CORE_THROW0()
#define CORE_THROW(x, y)

#else /* CORE_USE_EXCEPTIONS */

#define CORE_TRY_BEGIN		try {
#define CORE_CATCH(x)		} catch (x) {
#define CORE_CATCH_ALL		} catch (...) {
#define CORE_CATCH_END		}
#define CORE_RAISE(x)		throw x
#define CORE_RERAISE		throw
#define CORE_THROW0()		noexcept
#define CORE_THROW(x, y)	throw x(y)

#endif /* CORE_USE_EXCEPTIONS */

#ifdef CORE_NO_STD_TYPE_INFO
#include "core/type_info.h"
#else
#include <typeinfo>
#endif


