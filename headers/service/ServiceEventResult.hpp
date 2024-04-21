#pragma once

#define IS_CPP11 (201103L <= __cplusplus)

namespace webserv
{

#if !IS_CPP11
namespace ServiceEventResult
{
#endif

enum
#if IS_CPP11
	class
#endif
	ServiceEventResult {
		CONTINUE,
		COMPLETE,
		ERROR,
	};

#if !IS_CPP11
}	 // namespace ServiceEventResult
#endif

#if IS_CPP11
typedef ServiceEventResult ServiceEventResultType;
#else
typedef ServiceEventResult::ServiceEventResult ServiceEventResultType;
#endif

}	 // namespace webserv
