#include <unistd.h>

#include <cgi/CgiHandlerService.hpp>
#include <cstdio>
#include <iostream>
#include <macros.hpp>

namespace webserv
{

CgiHandlerService::CgiHandlerService(
	const HttpRequest &request,
	const utils::ErrorPageProvider &errorPageProvider,
	const Logger &logger,
	int fdReadFromCgi
) : ServiceBase(request, errorPageProvider, logger),
		_fdReadFromCgi(fdReadFromCgi)
{
}

CgiHandlerService::~CgiHandlerService()
{
	if (0 <= this->_fdReadFromCgi) {
		close(this->_fdReadFromCgi);
	}
}

void CgiHandlerService::setToPollFd(
	struct pollfd &pollFd
) const
{
	pollFd.fd = this->_fdReadFromCgi;
	pollFd.events = POLLIN;
	pollFd.revents = 0;
}

ServiceEventResultType CgiHandlerService::onEventGot(
	short revents
)
{
	if (!IS_POLLIN(revents)) {
		return ServiceEventResult::CONTINUE;
	}

	char buf[4096];
	ssize_t readResult = read(this->_fdReadFromCgi, buf, sizeof(buf));
	if (readResult < 0) {
		const errno_t errorNum = errno;
		CS_ERROR()
			<< "Failed to read from CGI: " << std::strerror(errorNum) << std::endl;

		return ServiceEventResult::ERROR;
	}

	if (readResult == 0) {
		LS_INFO() << "CGI read complete" << std::endl;
		return ServiceEventResult::COMPLETE;
	}

	CS_LOG() << "Read from CGI: " << std::string(buf, readResult) << std::endl;
	// TODO: CGIからのレスポンスを受け取る処理を実装する
	return ServiceEventResult::COMPLETE;
}

}	 // namespace webserv
