#pragma once

#include <limits>

#ifdef SIZE_MAX
#undef SIZE_MAX
#endif

namespace webserv
{

static const size_t SIZE_MAX = std::numeric_limits<size_t>::max();

}	 // namespace webserv
