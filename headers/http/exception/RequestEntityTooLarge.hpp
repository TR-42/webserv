#pragma once

#include <utils/ErrorPageProvider.hpp>

#include "HttpError.hpp"

namespace webserv
{

namespace http
{

namespace exception
{

class RequestEntityTooLarge : public HttpError
{
 public:
	RequestEntityTooLarge() {}
	RequestEntityTooLarge(const RequestEntityTooLarge &src) : HttpError(src) {}
	inline RequestEntityTooLarge &operator=(const RequestEntityTooLarge &src) throw()
	{
		HttpError::operator=(src);
		return *this;
	}
	virtual ~RequestEntityTooLarge() throw() {}

	virtual const char *what() const throw()
	{
		return "Request Entity Too Large";
	}

	virtual int getStatusCode() const throw()
	{
		return utils::ErrorPageProvider::REQUEST_ENTITY_TOO_LARGE;
	}
};

}	 // namespace exception

}	 // namespace http

}	 // namespace webserv
