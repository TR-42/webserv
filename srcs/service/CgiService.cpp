#include <service/CgiService.hpp>

namespace webserv
{

CgiService::CgiService(
	const HttpRequest &request,
	const utils::ErrorPageProvider &errorPageProvider,
	const Logger &logger,
	std::vector<Pollable *> &pollableList
) : ServiceBase(request, errorPageProvider, logger),
		_cgiExecuterService(NULL),
		_cgiHandlerService(NULL)
{
	(void)pollableList;
	(void)this->_cgiExecuterService;
	(void)this->_cgiHandlerService;
}

CgiService::~CgiService()
{
}

void CgiService::setToPollFd(
	struct pollfd &pollFd
) const
{
	(void)pollFd;
}

ServiceEventResultType CgiService::onEventGot(
	short revents
)
{
	(void)revents;
	return ServiceEventResult::CONTINUE;
}

}	 // namespace webserv
