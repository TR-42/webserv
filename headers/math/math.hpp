#pragma once

namespace webserv
{

namespace math
{

#define DECL_CAN_ADD(T) \
	bool can_add(T a, T b); \
	bool can_add(unsigned T a, unsigned T b);

DECL_CAN_ADD(int)
DECL_CAN_ADD(long)
DECL_CAN_ADD(long long)

#define DECL_CAN_MUL(T) \
	bool can_mul(T a, T b); \
	bool can_mul(unsigned T a, unsigned T b);

DECL_CAN_MUL(int)
DECL_CAN_MUL(long)
DECL_CAN_MUL(long long)

}	 // namespace math

}	 // namespace webserv
