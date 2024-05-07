#include <cstring>
#include <service/SimpleService.hpp>
#include <service/getRequestedFilePath.hpp>

namespace webserv
{

SimpleService::SimpleService(
	const HttpRequest &request,
	const HttpRouteConfig &routeConfig,
	const webserv::utils::ErrorPageProvider &errorPageProvider,
	const Logger &logger
) : ServiceBase(request, errorPageProvider, logger)
{
	std::string path = getRequestedFilePath(routeConfig, request.getNormalizedPath());
	if (path.empty() || path[0] != '/') {
		this->_response = this->_errorPageProvider.getErrorPage(
			400
		);
	} else {
		std::string errCode = path.substr(1);
		this->_response = this->_errorPageProvider.getErrorPage(
			errCode
		);
	}
}

SimpleService::~SimpleService()
{
}

void SimpleService::setToPollFd(
	pollfd &pollFd
) const
{
	// Simpleの場合は、fdを使わないため、無視設定を行う。
	std::memset(
		&pollFd,
		0,
		sizeof(pollFd)
	);
}

ServiceEventResultType SimpleService::onEventGot(
	short revents
)
{
	(void)revents;
	return ServiceEventResult::COMPLETE;
}

}	 // namespace webserv
