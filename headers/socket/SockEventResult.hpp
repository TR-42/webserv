#pragma once

#define IS_CPP11 (201103L <= __cplusplus)

namespace webserv
{

#if !IS_CPP11
namespace SockEventResult
{
#endif

enum
#if IS_CPP11
	class
#endif
	SockEventResult {
		OK,
		DISPOSE_REQUEST,
		ERROR,
	};

#if !IS_CPP11
}	 // namespace SockEventResult
#endif

#if IS_CPP11
typedef SockEventResult SockEventResultType;
#else
typedef SockEventResult::SockEventResult SockEventResultType;
#endif

}	 // namespace webserv
