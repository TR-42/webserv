#include <cstring>
#include <service/GetFileService.hpp>

namespace webserv
{

GetFileService::GetFileService(
	const HttpRequest &request,
	const webserv::utils::ErrorPageProvider &errorPageProvider,
	const Logger &logger
) : _request(request),
		_errorPageProvider(errorPageProvider),
		_logger(logger)
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

GetFileService::~GetFileService()
{
}

void GetFileService::setToPollFd(
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

ServiceEventResultType GetFileService::onEventGot(
	short revents
)
{
	(void)revents;
	return ServiceEventResult::COMPLETE;
}

}	 // namespace webserv
