#ifndef __PXF_UTIL_MAP_H__
#define __PXF_UTIL_MAP_H__

#include <map>

namespace Pxf {
namespace Util {

// Implement Hash-map later. R/B-tree is good enough for now. :]

template<typename K, typename V>
class Map : public std::map<K,V>
{

};

} // Util
} // Pxf

#endif //__PXF_UTIL_MAP_H__


