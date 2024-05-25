#include <unistd.h>

#include <cerrno>
#include <cgi/CgiHandler.hpp>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <macros.hpp>

namespace webserv
{

CgiHandler::CgiHandler(
	const utils::ErrorPageProvider &errorPageProvider,
	const Logger &logger,
	int fdReadFromCgi,
	CgiHandler **_cgiServiceCgiHandlerField,
	HttpResponse *_cgiServiceHttpResponseField
) : Pollable(fdReadFromCgi),
		logger(logger),
		_errorPageProvider(errorPageProvider),
		_isAnyResponseReceived(false),
		_cgiResponse(logger, errorPageProvider),
		_cgiServiceCgiHandlerField(_cgiServiceCgiHandlerField),
		_cgiServiceHttpResponseField(_cgiServiceHttpResponseField)
{
}

CgiHandler::CgiHandler(
	const CgiHandler &src
) : Pollable(src.getFD()),
		logger(src.logger),
		_errorPageProvider(src._errorPageProvider),
		_cgiResponse(src._cgiResponse)
{
	throw std::runtime_error("CgiHandler copy constructor is not allowed");
}

CgiHandler &CgiHandler::operator=(
	const CgiHandler &src
)
{
	(void)src;
	throw std::runtime_error("CgiHandler copy assignment operator is not allowed");
}

CgiHandler::~CgiHandler()
{
	if (this->_cgiServiceCgiHandlerField != NULL) {
		*(this->_cgiServiceCgiHandlerField) = NULL;
	}
	if (this->_cgiServiceHttpResponseField != NULL && this->_isAnyResponseReceived) {
		*(this->_cgiServiceHttpResponseField) = this->_cgiResponse.getHttpResponse();
	}
}

void CgiHandler::setToPollFd(
	struct pollfd &pollFd
) const
{
	Pollable::setToPollFd(pollFd);
	pollFd.events = POLLIN;
}

PollEventResultType CgiHandler::onEventGot(
	int fd,
	short revents,
	std::vector<Pollable *> &pollableList
)
{
	(void)fd;
	(void)pollableList;
	if (this->_cgiServiceCgiHandlerField == NULL || this->_cgiServiceHttpResponseField == NULL) {
		C_WARN("CgiHandler is not set to CGI service");
		return PollEventResult::DISPOSE_REQUEST;
	}
	if (!IS_POLLIN(revents)) {
		return PollEventResult::OK;
	}

	char buf[4096];
	ssize_t readResult = read(this->getFD(), buf, sizeof(buf));
	if (readResult < 0) {
		const errno_t errorNum = errno;
		CS_ERROR()
			<< "Failed to read from CGI: " << std::strerror(errorNum) << std::endl;

		*(this->_cgiServiceHttpResponseField) = this->_errorPageProvider.internalServerError();
		*(this->_cgiServiceCgiHandlerField) = NULL;
		this->_cgiServiceCgiHandlerField = NULL;
		this->_cgiServiceHttpResponseField = NULL;
		return PollEventResult::DISPOSE_REQUEST;
	}

	if (readResult == 0) {
		C_ERROR("CGI read complete");
		*(this->_cgiServiceHttpResponseField) = this->_cgiResponse.getHttpResponse();
		this->_cgiServiceCgiHandlerField = NULL;
		*(this->_cgiServiceCgiHandlerField) = NULL;
		return PollEventResult::DISPOSE_REQUEST;
	}

	CS_LOG() << "Read from CGI length: " << readResult << std::endl;
	this->_isAnyResponseReceived = true;
	this->_cgiResponse.pushResponseRaw(std::vector<uint8_t>(buf, buf + readResult));
	return PollEventResult::OK;
}

void CgiHandler::setDisposeRequested()
{
	this->_cgiServiceCgiHandlerField = NULL;
	this->_cgiServiceHttpResponseField = NULL;
}

}	 // namespace webserv
