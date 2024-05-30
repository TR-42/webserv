#include <iostream>
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
	CS_INFO()
		<< "Response: " << this->_response.getStatusCode() << " " << this->_response.getReasonPhrase()
		<< " (Content-Length: " << this->_response.getBody().size() << ")"
		<< std::endl;
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
