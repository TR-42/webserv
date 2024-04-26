#include <cstring>
#include <service/SimpleService.hpp>

namespace webserv
{

SimpleService::SimpleService(
	const HttpRequest &request,
	const webserv::utils::ErrorPageProvider &errorPageProvider,
	const Logger &logger
) : ServiceBase(request, errorPageProvider, logger)
{
	if (request.getPath().empty() || request.getPath()[0] != '/') {
		this->_response = this->_errorPageProvider.getErrorPage(
			400
		);
	} else {
		std::string errCode = request.getPath().substr(1);
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
