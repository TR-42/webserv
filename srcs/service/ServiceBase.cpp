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
		_isDisposingFromChildProcess(false)
{
}

ServiceBase::~ServiceBase()
{
}

const HttpResponse &ServiceBase::getResponse() const
{
	return this->_response;
}

bool ServiceBase::isDisposingFromChildProcess() const
{
	return this->_isDisposingFromChildProcess;
}

void ServiceBase::setIsDisposingFromChildProcess(bool value)
{
	this->_isDisposingFromChildProcess = value;
}

}	 // namespace webserv
