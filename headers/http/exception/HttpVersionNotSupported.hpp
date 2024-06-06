#pragma once

#include <utils/ErrorPageProvider.hpp>

#include "HttpError.hpp"

namespace webserv
{

namespace http
{

namespace exception
{

class HttpVersionNotSupported : public HttpError
{
 public:
	HttpVersionNotSupported() {}
	HttpVersionNotSupported(const HttpVersionNotSupported &src) : HttpError(src) {}
	inline HttpVersionNotSupported &operator=(const HttpVersionNotSupported &src) throw()
	{
		HttpError::operator=(src);
		return *this;
	}
	virtual ~HttpVersionNotSupported() throw() {}

	virtual const char *what() const throw()
	{
		return "HTTP Version Not Supported";
	}

	virtual int getStatusCode() const throw()
	{
		return utils::ErrorPageProvider::HTTP_VERSION_NOT_SUPPORTED;
	}
};

}	 // namespace exception

}	 // namespace http

}	 // namespace webserv
