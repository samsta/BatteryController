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

inline uint16_t limitScaledToUnsignedShort(float val, unsigned scale)
{
   if (val < 0) return 0;
   if (val*scale > float(UINT16_MAX)) return UINT16_MAX;
   return val*scale;
}

inline uint16_t limitScaledToSignedShort(float val, unsigned scale)
{
   if (val*scale < float(INT16_MIN)) return INT16_MIN;
   if (val*scale > float(INT16_MAX)) return INT16_MAX;
   return val*scale;
}

inline uint8_t limitValueToByte(unsigned val)
{
   if (val > 255) return 255;
   return val;
}

}



#endif // UTIL_HPP
