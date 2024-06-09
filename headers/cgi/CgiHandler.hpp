#pragma once

#include <Logger.hpp>
#include <poll/Pollable.hpp>
#include <types.hpp>
#include <utils/ErrorPageProvider.hpp>

#include "./CgiResponse.hpp"

namespace webserv
{

class CgiHandler : public Pollable
{
 private:
	Logger logger;
	uint8_t *_readBuf;
	const utils::ErrorPageProvider &_errorPageProvider;
	bool _isAnyResponseReceived;
	CgiResponse _cgiResponse;
	CgiHandler **_cgiServiceCgiHandlerField;
	HttpResponse *_cgiServiceHttpResponseField;
	bool *_isLocalRedirect;
	std::string *_localRedirectLocation;

	CgiHandler(const CgiHandler &src);
	CgiHandler &operator=(const CgiHandler &src);

 public:
	CgiHandler(
		const utils::ErrorPageProvider &errorPageProvider,
		const Logger &logger,
		int fdReadFromCgi,
		CgiHandler **_cgiServiceCgiHandlerField,
		HttpResponse *_cgiServiceHttpResponseField,
		bool *isLocalRedirect,
		std::string *localRedirectLocation
	);
	virtual ~CgiHandler();

	virtual void setToPollFd(
		struct pollfd &pollFd,
		const struct timespec &now
	) const;

	virtual PollEventResultType onEventGot(
		int fd,
		short revents,
		std::vector<Pollable *> &pollableList,
		const struct timespec &now
	);

	void setDisposeRequested();
};

}	 // namespace webserv
