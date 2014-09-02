#pragma once

#include "math.h"

template<class type>
inline type clamp(type x, type min, type max) {
	return x < min ? min : (x > max ? max : x);
}

template<class type>
inline bool valueFit(type val, type perfectValue, type allowedDiff) {
	return val > (perfectValue - allowedDiff) && val < (perfectValue + allowedDiff);
}
