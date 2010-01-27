#ifndef __PXF_UTIL_NONCOPYABLE_H__
#define __PXF_UTIL_NONCOPYABLE_H__

namespace Pxf {
namespace Util {

// protect from unintended ADL
namespace Noncopyable_
{
	class Noncopyable
	{
	protected:
		Noncopyable(){};
		~Noncopyable(){};
	private:
		Noncopyable(const Noncopyable&);
		const Noncopyable& operator=(const Noncopyable&);
	};
}
typedef Noncopyable_::Noncopyable Noncopyable;

} // Util
} // Pxf

#endif //__PXF_UTIL_NONCOPYABLE_H__


