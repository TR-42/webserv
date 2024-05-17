#pragma once

#define IS_CPP11 (201103L <= __cplusplus)

namespace webserv
{

#if !IS_CPP11
namespace CgiResponseMode
{
#endif

enum
#if IS_CPP11
	class
#endif
	CgiResponseMode {
		DOCUMENT,
		LOCAL_REDIRECT,
		CLIENT_REDIRECT,
		CLIENT_REDIRECT_WITH_DOCUMENT,
	};

#if !IS_CPP11
}	 // namespace CgiResponseMode
#endif

#if IS_CPP11
typedef CgiResponseMode CgiResponseModeType;
#else
typedef CgiResponseMode::CgiResponseMode CgiResponseModeType;
#endif

}	 // namespace webserv
