#pragma once

#include <utils/UUID.hpp>

#include "../Logger.hpp"
#include "../http/HttpRequest.hpp"
#include "./Socket.hpp"

namespace webserv
{

class ClientSocket : public Socket
{
 private:
	Logger logger;
	HttpRequest httpRequest;
	std::vector<uint8_t> httpResponseBuffer;

	SockEventResultType _processPollIn();
	SockEventResultType _processPollOut();

 public:
	ClientSocket(
		int fd,
		const std::string &serverLoggerCustomId
	);
	virtual ~ClientSocket();

	virtual SockEventResultType onEventGot(
		short revents,
		std::vector<Socket *> &sockets
	);
};

}	 // namespace webserv
