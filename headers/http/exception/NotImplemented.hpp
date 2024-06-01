#pragma once

#include <utils/ErrorPageProvider.hpp>

#include "HttpError.hpp"

namespace webserv
{

namespace http
{

namespace exception
{

class NotImplemented : public HttpError
{
 public:
	NotImplemented() {}
	NotImplemented(const NotImplemented &src) : HttpError(src) {}
	inline NotImplemented &operator=(const NotImplemented &src) throw()
	{
		HttpError::operator=(src);
		return *this;
	}
	virtual ~NotImplemented() throw() {}

	virtual const char *what() const throw()
	{
		return "Not Implemented";
	}

	virtual int getStatusCode() const throw()
	{
		return utils::ErrorPageProvider::NOT_IMPLEMENTED;
	}
};

}	 // namespace exception

}	 // namespace http

}	 // namespace webserv
