#include <climits>

#include "math/math.hpp"

#define IMPL_CAN_ADD(T, T_MIN, T_MAX, UT_MAX) \
	bool webserv::math::can_add(T a, T b) \
	{ \
		if ((a <= 0 && 0 <= b) || (b <= 0 && 0 <= a)) \
			return (true); \
		else if (0 < a) \
			return (b <= (T_MAX - a)); \
		else \
			return ((T_MIN - a) <= b); \
	} \
	bool webserv::math::can_add(unsigned T a, unsigned T b) \
	{ \
		return (b <= (UT_MAX - a)); \
	}

IMPL_CAN_ADD(int, INT_MIN, INT_MAX, UINT_MAX)
IMPL_CAN_ADD(long, LONG_MIN, LONG_MAX, ULONG_MAX)
IMPL_CAN_ADD(long long, LLONG_MIN, LLONG_MAX, ULLONG_MAX)
