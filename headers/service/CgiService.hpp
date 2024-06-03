#pragma once

#include <arpa/inet.h>
#include <unistd.h>

#include <EnvManager.hpp>
#include <cgi/CgiExecuter.hpp>
#include <cgi/CgiHandler.hpp>
#include <service/RequestedFileInfo.hpp>
#include <service/ServiceBase.hpp>
#include <types.hpp>

namespace webserv
{

class CgiService : public ServiceBase
{
 private:
	pid_t _pid;
	CgiExecuter *_cgiExecuter;
	CgiHandler *_cgiHandler;
	bool _isLocalRedirect;
	std::string _localRedirectLocation;

 public:
	CgiService(
		const HttpRequest &request,
		const RequestedFileInfo &requestedFileInfo,
		uint16_t serverPort,
		const struct sockaddr &clientAddr,
		const utils::ErrorPageProvider &errorPageProvider,
		const Logger &logger,
		std::vector<Pollable *> &pollableList
	);
	~CgiService();

	virtual void setToPollFd(
		struct pollfd &pollFd
	) const;

	virtual ServiceEventResultType onEventGot(
		short revents
	);

	inline bool isLocalRedirect() const
	{
		return this->_isLocalRedirect;
	}
	inline std::string getLocalRedirectLocation() const
	{
		return this->_localRedirectLocation;
	}
};

};	// namespace webserv
