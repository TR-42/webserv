#pragma once

#include <utils/ErrorPageProvider.hpp>

#include "./HttpError.hpp"

namespace webserv
{

namespace http
{

namespace exception
{

class BadRequest : public HttpError
{
 public:
	BadRequest() {}
	BadRequest(const BadRequest &src) : HttpError(src) {}
	inline BadRequest &operator=(const BadRequest &src) throw()
	{
		HttpError::operator=(src);
		return *this;
	}
	virtual ~BadRequest() throw() {}

	virtual const char *what() const throw()
	{
		return "Bad Request";
	}

	virtual int getStatusCode() const throw()
	{
		return utils::ErrorPageProvider::BAD_REQUEST;
	}
};

}	 // namespace exception

}	 // namespace http

}	 // namespace webserv
