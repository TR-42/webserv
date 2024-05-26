#include <cstring>
#include <service/SimpleService.hpp>
#include <service/getRequestedFilePath.hpp>

namespace webserv
{

SimpleService::SimpleService(
	const HttpRequest &request,
	const HttpResponse &response,
	const webserv::utils::ErrorPageProvider &errorPageProvider,
	const Logger &logger
) : ServiceBase(request, errorPageProvider, logger)
{
	this->_response = response;
}

SimpleService::~SimpleService()
{
}

void SimpleService::setToPollFd(
	pollfd &pollFd
) const
{
	// Simpleの場合は、fdを使わないため、無視設定を行う。
	pollFd.fd = -1;
	pollFd.events = 0;
	pollFd.revents = 0;
}

ServiceEventResultType SimpleService::onEventGot(
	short revents
)
{
	(void)revents;
	return ServiceEventResult::COMPLETE;
}

}	 // namespace webserv
