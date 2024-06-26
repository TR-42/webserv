#pragma once

#include <arpa/inet.h>

#include <config/ListenConfig.hpp>
#include <config/ServerRunningConfig.hpp>
#include <poll/Pollable.hpp>
#include <service/SimpleService.hpp>
#include <utils/UUID.hpp>

#include "../Logger.hpp"
#include "../http/HttpRequest.hpp"
#include "../http/HttpResponse.hpp"
#include "./TimeoutChecker.hpp"

namespace webserv
{

class ClientSocket : public Pollable
{
 private:
	const ServerRunningConfigListType &_listenConfigList;
	Logger logger;
	uint8_t *_readBuf;
	HttpRequest httpRequest;
	std::vector<uint8_t> httpResponseBuffer;
	size_t _responseBufferOffset;
	bool _IsResponseSet;
	ServiceBase *_service;
	struct sockaddr _clientAddr;
	bool _IsHeaderValidationCompleted;
	TimeoutChecker _timeoutChecker;
	bool _IsEofSent;

	PollEventResultType _processPollIn(
		const struct timespec &now,
		std::vector<Pollable *> &pollableList
	);
	PollEventResultType _processPollOut();
	void _processPollService(
		short revents,
		std::vector<Pollable *> &pollableList
	);

	void _setResponse(
		const std::vector<uint8_t> &response
	);
	void _setResponse(
		const std::string &responseStr
	);
	void _setResponse(
		const HttpResponse &response
	);

 public:
	ClientSocket(
		int fd,
		const utils::UUID &uuid,
		const struct sockaddr &clientAddr,
		const timespec &now,
		const ServerRunningConfigListType &listenConfigList,
		const Logger &logger
	);
	virtual ~ClientSocket();

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
};

}	 // namespace webserv
