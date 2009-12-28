#ifndef __PXF_BASE_DEBUG_H__
#define __PXF_BASE_DEBUG_H__

#include <Pxf/Base/Config.h>

namespace Pxf
{
	#define PxfStaticAssert(e, m) typedef \
	struct { int _PXF_STATIC_ASSERT_NAME_(__LINE__): ((e) ? 1 : -1); } \
	_PXF_STATIC_ASSERT_NAME_(__LINE__)[(e) ? 1 : -1]
	#define _PXF_STATIC_ASSERT_NAME_(line)	_PXF_STATIC_ASSERT_NAME2_(line)
	#define _PXF_STATIC_ASSERT_NAME2_(line)	static_assert_failed_at_line_##line


/**
 * Example
 *
 *
	struct timer {
	  uint8_t MODE;
	  uint32_t DATA;
	  uint32_t COUNT;
	};

	static_assert(offsetof(struct timer, DATA) == 4);
*/
}

#endif //__PXF_BASE_DEBUG_H__

