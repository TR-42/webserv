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
 private:
	std::string reason;

 public:
	NotImplemented() : reason("Not Implemented") {}
	NotImplemented(const std::string &reason) : reason("Not Implemented")
	{
		if (!reason.empty()) {
			this->reason += ": ";
			this->reason += reason;
		}
	}
	NotImplemented(const NotImplemented &src) : HttpError(src), reason(src.reason) {}
	inline NotImplemented &operator=(const NotImplemented &src) throw()
	{
		HttpError::operator=(src);
		if (this == &src) {
			return *this;
		}

		this->reason = src.reason;
		return *this;
	}
	virtual ~NotImplemented() throw() {}

	virtual const char *what() const throw()
	{
		return this->reason.c_str();
	}

	virtual int getStatusCode() const throw()
	{
		return utils::ErrorPageProvider::NOT_IMPLEMENTED;
	}
};

}	 // namespace exception

}	 // namespace http

}	 // namespace webserv
