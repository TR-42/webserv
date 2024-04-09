#include <climits>

#include "math/math.hpp"

#define IMPL_CAN_MUL(T, T_MIN, T_MAX, UT_MAX) \
	bool webserv::math::can_mul(T a, T b) \
	{ \
		if (a == 0 || b == 0) \
			return (true); \
		else if ((0 < a && 0 < b) || (a < 0 && b < 0)) \
			return (b <= (T_MAX / a)); \
		else if (b < 0) \
			return ((T_MIN / a) <= b); \
		else \
			return ((T_MIN / b) <= a); \
	} \
	bool webserv::math::can_mul(unsigned T a, unsigned T b) \
	{ \
		if (a == 0 || b == 0) \
			return (true); \
		else \
			return (b <= (UT_MAX / a)); \
	}

IMPL_CAN_MUL(int, INT_MIN, INT_MAX, UINT_MAX)
IMPL_CAN_MUL(long, LONG_MIN, LONG_MAX, ULONG_MAX)
IMPL_CAN_MUL(long long, LLONG_MIN, LLONG_MAX, ULLONG_MAX)
