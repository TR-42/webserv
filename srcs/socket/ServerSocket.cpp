#include <arpa/inet.h>
#include <sys/socket.h>

#include <socket/ClientSocket.hpp>
#include <socket/ServerSocket.hpp>
#include <utils.hpp>
#include <utils/UUIDv7.hpp>

namespace webserv
{

SockEventResultType ServerSocket::onEventGot(
	short revents,
	std::vector<Socket *> &sockets
)
{
	if (!IS_POLLIN(revents)) {
		CS_DEBUG()
			<< "No POLLIN event"
			<< std::endl;
		return SockEventResult::OK;
	}

	struct sockaddr clientAddr;
	socklen_t clientAddrLen = 0;
	int clientFd = accept(
		this->getFD(),
		&clientAddr,
		&clientAddrLen
	);
	if (clientFd < 0) {
		const char *errorStr = strerror(errno);
		CS_FATAL()
			<< "accept() failed: " << errorStr
			<< std::endl;
		return SockEventResult::ERROR;
	}

	CS_INFO()
		<< "Accepted new connection from "
		<< utils::to_string(clientAddr)
		<< " (port: " << ntohs(((struct sockaddr_in *)&clientAddr)->sin_port) << ")"
		<< std::endl;

	utils::UUID clientUuid = utils::UUIDv7();
	Socket *clientSocket = new ClientSocket(clientFd, this->logger.getCustomId());
	sockets.push_back(clientSocket);
	CS_DEBUG()
		<< "New client socket created: " << clientUuid
		<< std::endl;

	return SockEventResult::OK;
}

void ServerSocket::setToPollFd(
	struct pollfd &pollFd
) const
{
	Socket::setToPollFd(pollFd);
	pollFd.events = POLLIN;
}

ServerSocket::ServerSocket(
	int fd,
	const Logger &logger
) : Socket(fd),
		logger(logger, "Server=" + Socket::getUUID().toString())
{
}

ServerSocket::~ServerSocket()
{
	// serverFdのcloseは、Socketのデストラクタで行われる
	// clientSocketのDisposeは、socketListから削除するときに行われる
}

}	 // namespace webserv
