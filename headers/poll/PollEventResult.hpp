#pragma once

#define IS_CPP11 (201103L <= __cplusplus)

namespace webserv
{

#if !IS_CPP11
namespace PollEventResult
{
#endif

enum
#if IS_CPP11
	class
#endif
	PollEventResult {
		OK,
		DISPOSE_REQUEST,
	};

#if !IS_CPP11
}	 // namespace PollEventResult
#endif

#if IS_CPP11
typedef PollEventResult PollEventResultType;
#else
typedef PollEventResult::PollEventResult PollEventResultType;
#endif

}	 // namespace webserv
