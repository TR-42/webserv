#pragma once

#include <cgi/CgiExecuter.hpp>
#include <cgi/CgiHandler.hpp>
#include <service/ServiceBase.hpp>

namespace webserv
{

class CgiService : public ServiceBase
{
 private:
	CgiExecuter *_cgiExecuter;
	CgiHandler *_cgiHandler;

 public:
	CgiService(
		const HttpRequest &request,
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
};

};	// namespace webserv
