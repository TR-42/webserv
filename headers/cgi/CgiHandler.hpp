#pragma once

#include <Logger.hpp>
#include <poll/Pollable.hpp>
#include <utils/ErrorPageProvider.hpp>

namespace webserv
{

class CgiHandler : public Pollable
{
 private:
	Logger logger;
	const utils::ErrorPageProvider &_errorPageProvider;
	HttpResponse _response;
	bool _isResponseReady;
	bool _isDisposeRequested;

 public:
	CgiHandler(
		const utils::ErrorPageProvider &errorPageProvider,
		const Logger &logger,
		int fdReadFromCgi
	);
	virtual ~CgiHandler();

	virtual void setToPollFd(
		struct pollfd &pollFd
	) const;

	virtual PollEventResultType onEventGot(
		short revents,
		std::vector<Pollable *> &pollableList
	);

	bool isResponseReady() const;
	HttpResponse getResponse() const;
	void setDisposeRequested(bool value);

	// TODO: Redirect対応
};

}	 // namespace webserv
