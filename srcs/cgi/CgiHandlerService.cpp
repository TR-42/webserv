#include <unistd.h>

#include <cgi/CgiHandlerService.hpp>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <macros.hpp>

namespace webserv
{

CgiHandlerService::CgiHandlerService(
	const utils::ErrorPageProvider &errorPageProvider,
	const Logger &logger,
	int fdReadFromCgi
) : Pollable(fdReadFromCgi),
		logger(logger)
{
	(void)errorPageProvider;
}

CgiHandlerService::~CgiHandlerService()
{
}

void CgiHandlerService::setToPollFd(
	struct pollfd &pollFd
) const
{
	Pollable::setToPollFd(pollFd);
	pollFd.events = POLLIN;
}

PollEventResultType CgiHandlerService::onEventGot(
	short revents,
	std::vector<Pollable *> &pollableList
)
{
	(void)pollableList;
	if (!IS_POLLIN(revents)) {
		return PollEventResult::OK;
	}

	char buf[4096];
	ssize_t readResult = read(this->getFD(), buf, sizeof(buf));
	if (readResult < 0) {
		const errno_t errorNum = errno;
		CS_ERROR()
			<< "Failed to read from CGI: " << std::strerror(errorNum) << std::endl;

		return PollEventResult::ERROR;
	}

	if (readResult == 0) {
		LS_INFO() << "CGI read complete" << std::endl;
		return PollEventResult::DISPOSE_REQUEST;
	}

	CS_LOG() << "Read from CGI: " << std::string(buf, readResult) << std::endl;
	// TODO: CGIからのレスポンスを受け取る処理を実装する
	return PollEventResult::DISPOSE_REQUEST;
}

}	 // namespace webserv
