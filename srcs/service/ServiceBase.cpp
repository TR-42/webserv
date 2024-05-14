#include <service/ServiceBase.hpp>

namespace webserv
{

ServiceBase::ServiceBase(
	const HttpRequest &request,
	const webserv::utils::ErrorPageProvider &errorPageProvider,
	const Logger &logger
) : _request(request),
		_errorPageProvider(errorPageProvider),
		logger(logger),
		_canDispose(false)
{
}

ServiceBase::~ServiceBase()
{
}

const HttpResponse &ServiceBase::getResponse() const
{
	return this->_response;
}

bool ServiceBase::canDispose() const
{
	return this->_canDispose;
}

}	 // namespace webserv
