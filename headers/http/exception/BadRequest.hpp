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
 private:
	std::string reason;

 public:
	BadRequest() : reason("Bad Request") {}
	BadRequest(const std::string &reason) : reason("Bad Request")
	{
		if (!reason.empty()) {
			this->reason += ": ";
			this->reason += reason;
		}
	}
	BadRequest(const BadRequest &src) : HttpError(src), reason(src.reason) {}
	inline BadRequest &operator=(const BadRequest &src) throw()
	{
		HttpError::operator=(src);
		if (this == &src) {
			return *this;
		}

		this->reason = src.reason;
		return *this;
	}
	virtual ~BadRequest() throw() {}

	virtual const char *what() const throw()
	{
		return this->reason.c_str();
	}

	virtual int getStatusCode() const throw()
	{
		return utils::ErrorPageProvider::BAD_REQUEST;
	}
};

}	 // namespace exception

}	 // namespace http

}	 // namespace webserv
