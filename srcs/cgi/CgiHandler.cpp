#include <unistd.h>

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
	int fdReadFromCgi
) : Pollable(fdReadFromCgi),
		logger(logger),
		_errorPageProvider(errorPageProvider),
		_response(),
		_isResponseReady(false),
		_isDisposeRequested(false),
		_cgiResponse(logger)
{
	this->_response = this->_errorPageProvider.notImplemented();
}

CgiHandler::~CgiHandler()
{
}

void CgiHandler::setToPollFd(
	struct pollfd &pollFd
) const
{
	Pollable::setToPollFd(pollFd);
	pollFd.events = POLLIN;
}

PollEventResultType CgiHandler::onEventGot(
	short revents,
	std::vector<Pollable *> &pollableList
)
{
	(void)pollableList;
	if (this->_isDisposeRequested) {
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

		this->_response = this->_errorPageProvider.internalServerError();
		this->_isResponseReady = true;
		return PollEventResult::OK;
	}

	if (readResult == 0) {
		LS_ERROR() << "CGI read complete" << std::endl;
		this->_response = this->_cgiResponse.getHttpResponse();
		this->_isResponseReady = true;
		return PollEventResult::OK;
	}

	CS_LOG() << "Read from CGI length: " << readResult << std::endl;
	this->_cgiResponse.pushResponseRaw(std::vector<uint8_t>(buf, buf + readResult));
	return PollEventResult::OK;
}

bool CgiHandler::isResponseReady() const
{
	return this->_isResponseReady;
}

HttpResponse CgiHandler::getResponse() const
{
	if (!this->_isResponseReady) {
		return this->_cgiResponse.getHttpResponse();
	}
	return this->_response;
}

void CgiHandler::setDisposeRequested(bool value)
{
	this->_isDisposeRequested = value;
}

}	 // namespace webserv
