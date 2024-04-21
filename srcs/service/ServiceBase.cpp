#include <cstring>
#include <service/SimpleService.hpp>

namespace webserv
{

ServiceBase::ServiceBase(
	const HttpRequest &request,
	const webserv::utils::ErrorPageProvider &errorPageProvider,
	const Logger &logger
) : _request(request),
		_errorPageProvider(errorPageProvider),
		_logger(logger)
{
}

ServiceBase::~ServiceBase()
{
}

const HttpResponse &ServiceBase::getResponse() const
{
	return this->_response;
}

}	 // namespace webserv
