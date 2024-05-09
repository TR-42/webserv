#include <unistd.h>

#include <cgi/CgiExecuterService.hpp>
#include <iostream>
#include <macros.hpp>
#include <stdexcept>

namespace webserv
{

CgiExecuterService::CgiExecuterService(
	const HttpRequest &request,
	const utils::ErrorPageProvider &errorPageProvider,
	const Logger &logger
) : ServiceBase(request, errorPageProvider, logger),
		_fdWriteToCgi(-1),
		_fdReadFromCgi(-1),
		_isReaderInstanceCreated(false)
{
}

CgiExecuterService::~CgiExecuterService()
{
	if (0 <= this->_fdWriteToCgi) {
		close(this->_fdWriteToCgi);
	}
}

void CgiExecuterService::setToPollFd(
	pollfd &pollFd
) const
{
	pollFd.fd = this->_fdWriteToCgi;
	pollFd.events = POLLOUT;
	pollFd.revents = 0;
}

ServiceEventResultType CgiExecuterService::onEventGot(
	short revents
)
{
	if (this->_fdWriteToCgi < 0 || this->_fdReadFromCgi < 0) {
		CS_ERROR()
			<< "invalid file descriptors: "
			<< "writer=" << this->_fdWriteToCgi
			<< ", reader=" << this->_fdReadFromCgi
			<< std::endl;
		return ServiceEventResult::ERROR;
	}

	if (!IS_POLLOUT(revents)) {
		return ServiceEventResult::CONTINUE;
	}

	// TODO: implement

	// readerのcompleteによって、自動的にwriterもcompleteになる
	return ServiceEventResult::CONTINUE;
}

bool CgiExecuterService::isWriterInstance() const
{
	return true;
}

CgiHandlerService *CgiExecuterService::createCgiHandlerService()
{
	if (this->_isReaderInstanceCreated) {
		CS_ERROR() << "CgiHandlerService instance already created" << std::endl;
		throw std::runtime_error("CgiHandlerService instance already created");
		return NULL;
	}

	this->_isReaderInstanceCreated = true;
	return new CgiHandlerService(
		this->_request,
		this->_errorPageProvider,
		this->logger,
		this->_fdReadFromCgi
	);
}

}	 // namespace webserv
