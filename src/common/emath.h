#pragma once

#include "math.h"
/** \brief will cut value from both sides of number line, causing it fit in supplied boundaries
*
* \param x value that will be fitted in boundaries or returend in case it already fit.
* \param min minimum value, value can't be less than it.
* \param max maximum value, value can't be more than it.
*
* \returns value that is x when min <= x <= max, min when x < min, and max when x > max.
*/
template<class type>
inline type clamp(type x, type min, type max) {
	return x < min ? min : (x > max ? max : x);
}

/** \brief checks if value is near enough(or the same) to some other value
*
* \param val value to check
* \param perfectValue value to which val should be near
* \param allowedDiff allowed deviation from perfectValue
*
* \returns true if value is near enough to perfectValue, false otherwise.
*/
template<class type>
inline bool valueFit(type val, type perfectValue, type allowedDiff) {
	return val > (perfectValue - allowedDiff) && val < (perfectValue + allowedDiff);
}
