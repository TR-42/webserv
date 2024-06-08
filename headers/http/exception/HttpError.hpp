#pragma once

#include <Logger.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utils/ErrorPageProvider.hpp>

#include "../HttpResponse.hpp"

namespace webserv
{

namespace http
{

namespace exception
{

class HttpError : public std::exception
{
 public:
	HttpError() {}
	HttpError(const HttpError &src) : std::exception(src) {}
	HttpError &operator=(const HttpError &src) throw()
	{
		std::exception::operator=(src);
		return *this;
	}
	virtual ~HttpError() throw() {}

	virtual const char *what() const throw() = 0;

	virtual int getStatusCode() const throw() = 0;

	HttpResponse toResponse(
		const utils::ErrorPageProvider &provider,
		const Logger &logger
	) const throw()
	{
		try {
			return provider.getErrorPage(this->getStatusCode());
		} catch (const std::exception &e) {
			LS_ERROR()
				<< "Error while generating error page: "
				<< e.what()
				<< std::endl;
			return provider.internalServerError();
		}
	}
};

}	 // namespace exception

}	 // namespace http

}	 // namespace webserv
