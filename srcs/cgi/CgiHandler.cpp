#include <unistd.h>

#include <cerrno>
#include <cgi/CgiHandler.hpp>
#include <cstring>
#include <iostream>
#include <macros.hpp>

#define READ_BUF_SIZE (256 * 256 * 256)

namespace webserv
{

CgiHandler::CgiHandler(
	const utils::ErrorPageProvider &errorPageProvider,
	const Logger &logger,
	int fdReadFromCgi,
	CgiHandler **_cgiServiceCgiHandlerField,
	HttpResponse *_cgiServiceHttpResponseField,
	bool *isLocalRedirect,
	std::string *localRedirectLocation
) : Pollable(fdReadFromCgi),
		logger(logger),
		_readBuf(NULL),
		_errorPageProvider(errorPageProvider),
		_isAnyResponseReceived(false),
		_cgiResponse(logger, errorPageProvider),
		_cgiServiceCgiHandlerField(_cgiServiceCgiHandlerField),
		_cgiServiceHttpResponseField(_cgiServiceHttpResponseField),
		_isLocalRedirect(isLocalRedirect),
		_localRedirectLocation(localRedirectLocation)
{
}

CgiHandler::CgiHandler(
	const CgiHandler &src
) : Pollable(src.getFD()),
		logger(src.logger),
		_readBuf(NULL),
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
	C_DEBUG("CgiHandler destructor");
	if (this->_cgiServiceCgiHandlerField != NULL) {
		*(this->_cgiServiceCgiHandlerField) = NULL;
	}

	bool isLocalRedirect = this->_cgiResponse.getMode() == CgiResponseMode::LOCAL_REDIRECT;
	if (this->_isLocalRedirect != NULL && this->_localRedirectLocation != NULL) {
		*(this->_isLocalRedirect) = isLocalRedirect;
		*(this->_localRedirectLocation) = this->_cgiResponse.getLocation();
	}
	if (!isLocalRedirect && this->_cgiServiceHttpResponseField != NULL && this->_isAnyResponseReceived) {
		*(this->_cgiServiceHttpResponseField) = this->_cgiResponse.getHttpResponse();
	}
}

void CgiHandler::setToPollFd(
	struct pollfd &pollFd,
	const struct timespec &now
) const
{
	Pollable::setToPollFd(pollFd, now);
	pollFd.events = POLLIN;
}

PollEventResultType CgiHandler::onEventGot(
	int fd,
	short revents,
	std::vector<Pollable *> &pollableList,
	const struct timespec &now
)
{
	(void)fd;
	(void)pollableList;
	(void)now;
	if (this->_cgiServiceCgiHandlerField == NULL || this->_cgiServiceHttpResponseField == NULL) {
		C_WARN("CgiHandler is not set to CGI service");
		return PollEventResult::DISPOSE_REQUEST;
	}
	if (!IS_POLLIN(revents)) {
		return PollEventResult::OK;
	}

	if (this->_readBuf == NULL) {
		try {
			this->_readBuf = new uint8_t[READ_BUF_SIZE];
		} catch (const std::exception &e) {
			CS_ERROR()
				<< "Failed to allocate memory for CGI read buffer: " << e.what() << std::endl;

			*(this->_cgiServiceHttpResponseField) = this->_errorPageProvider.internalServerError();
			*(this->_cgiServiceCgiHandlerField) = NULL;
			this->_cgiServiceCgiHandlerField = NULL;
			this->_cgiServiceHttpResponseField = NULL;
			return PollEventResult::DISPOSE_REQUEST;
		}
	}
	ssize_t readResult = read(this->getFD(), this->_readBuf, READ_BUF_SIZE);
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
		if (this->_cgiResponse.getMode() == CgiResponseMode::LOCAL_REDIRECT) {
			*(this->_isLocalRedirect) = true;
			*(this->_localRedirectLocation) = this->_cgiResponse.getLocation();
		} else {
			*(this->_cgiServiceHttpResponseField) = this->_cgiResponse.getHttpResponse();
		}
		*(this->_cgiServiceCgiHandlerField) = NULL;
		this->_cgiServiceCgiHandlerField = NULL;
		this->_cgiServiceHttpResponseField = NULL;
		this->_isLocalRedirect = NULL;
		this->_localRedirectLocation = NULL;
		return PollEventResult::DISPOSE_REQUEST;
	}

	CS_LOG() << "Read from CGI length: " << readResult << std::endl;
	this->_isAnyResponseReceived = true;
	this->_cgiResponse.pushResponseRaw(this->_readBuf, readResult);
	return PollEventResult::OK;
}

void CgiHandler::setDisposeRequested()
{
	C_DEBUG("CgiHandler::setDisposeRequested()");
	if (this->_readBuf != NULL) {
		delete[] this->_readBuf;
		this->_readBuf = NULL;
	}
	this->_cgiServiceCgiHandlerField = NULL;
	this->_cgiServiceHttpResponseField = NULL;
	this->_isLocalRedirect = NULL;
	this->_localRedirectLocation = NULL;
}

}	 // namespace webserv
