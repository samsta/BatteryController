#ifndef UTIL_HPP
#define UTIL_HPP

/*
 * Lots of stuff in here purely exist because the Arduino doesn't have
 * STL support
 */
namespace util {
   
template <class T> inline const T& max(const T& a, const T& b) {
   return (a < b) ? b : a;
}

template <class T> inline const T& min(const T& a, const T& b) {
   return (a > b) ? b : a;
}

template <class T> inline const T& clamp(const T& v, const T& minv, const T& maxv) {
   return min(max(v, minv), maxv);
}  
   
}

#endif // UTIL_HPP
