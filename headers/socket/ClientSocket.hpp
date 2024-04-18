#pragma once

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
	Logger logger;
	HttpRequest httpRequest;
	std::vector<uint8_t> httpResponseBuffer;
	bool _IsResponseSet;

	SockEventResultType _processPollIn();
	SockEventResultType _processPollOut();

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
		const std::string &serverLoggerCustomId
	);
	virtual ~ClientSocket();

	virtual void setToPollFd(
		struct pollfd &pollFd
	) const;

	virtual SockEventResultType onEventGot(
		short revents,
		std::vector<Socket *> &sockets
	);
};

}	 // namespace webserv
