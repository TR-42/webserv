#pragma once

#include <config/ListenConfig.hpp>
#include <service/SimpleService.hpp>
#include <utils/UUID.hpp>

#include "../Logger.hpp"
#include "../http/HttpRequest.hpp"
#include "../http/HttpResponse.hpp"
#include "./Socket.hpp"

namespace webserv
{

class ClientSocket : public Socket
{
 private:
	const ServerConfigListType &_listenConfigList;
	Logger logger;
	HttpRequest httpRequest;
	std::vector<uint8_t> httpResponseBuffer;
	bool _IsResponseSet;
	SimpleService *_service;

	SockEventResultType _processPollIn();
	SockEventResultType _processPollOut();
	SockEventResultType _processPollService(short revents);

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
		const std::string &serverLoggerCustomId,
		const ServerConfigListType &listenConfigList
	);
	virtual ~ClientSocket();

	virtual void setToPollFd(
		struct pollfd &pollFd
	) const;

	virtual SockEventResultType onEventGot(
		short revents,
		std::vector<Socket *> &sockets
	);
	SockEventResultType onEventGot(
		short revents
	);
};

}	 // namespace webserv
