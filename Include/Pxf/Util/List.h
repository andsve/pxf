#ifndef __PXF_UTIL_LIST_H__
#define __PXF_UTIL_LIST_H__

#include <list>

namespace Pxf {
namespace Util {

template<typename T>
class List : public std::list<T>
{

};

} // Util
} // Pxf

#endif //__PXF_UTIL_LIST_H__


