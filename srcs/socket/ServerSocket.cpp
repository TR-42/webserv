#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <macros.hpp>
#include <socket/ClientSocket.hpp>
#include <socket/ServerSocket.hpp>
#include <utils/UUIDv7.hpp>
#include <utils/to_string.hpp>

// 適当に決めた値
#define ACCEPT_BACKLOG 16

namespace webserv
{

PollEventResultType ServerSocket::onEventGot(
	int fd,
	short revents,
	std::vector<Pollable *> &pollableList,
	const struct timespec &now
)
{
	(void)fd;
	if (IS_POLL_ANY_ERROR(revents)) {
		// ServerSocketがエラーになるはずは無いが、一応エラー処理を入れておく
		CS_ERROR()
			<< "Error event"
			<< std::endl;
		return PollEventResult::DISPOSE_REQUEST;
	}

	if (!IS_POLLIN(revents)) {
		return PollEventResult::OK;
	}

	struct sockaddr clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	int clientFd = accept(
		this->getFD(),
		&clientAddr,
		&clientAddrLen
	);
	if (clientFd < 0) {
		const char *errorStr = std::strerror(errno);
		CS_FATAL()
			<< "accept() failed: " << errorStr
			<< std::endl;
		return PollEventResult::DISPOSE_REQUEST;
	}

	utils::UUID clientUuid = utils::UUIDv7();
	CS_INFO()
		<< "Accepted new connection from "
		<< utils::to_string(clientAddr)
		<< " (port: " << ntohs(((struct sockaddr_in *)&clientAddr)->sin_port) << ")"
		<< " len: " << clientAddrLen
		<< " family: " << (int)(clientAddr.sa_family)
		<< " fd: " << clientFd
		<< " UUID: " << clientUuid.toString()
		<< std::endl;

	Pollable *clientSocket = new ClientSocket(
		clientFd,
		clientUuid,
		clientAddr,
		now,
		this->_listenConfigList,
		this->logger
	);
	pollableList.push_back(clientSocket);
	CS_DEBUG()
		<< "New client socket created: " << clientUuid
		<< std::endl;

	return PollEventResult::OK;
}

void ServerSocket::setToPollFd(
	struct pollfd &pollFd,
	const struct timespec &now
) const
{
	Pollable::setToPollFd(pollFd, now);
	pollFd.events = POLLIN;
}

ServerSocket *ServerSocket::createServerSocket(
	const ServerRunningConfigListType &listenConfigList,
	uint16_t port,
	const Logger &logger
)
{
	if (listenConfigList.size() == 0) {
		L_FATAL("listenConfigList is empty");
		return NULL;
	}

	struct sockaddr_in addr;

	// listenするのはIPv4のみ
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (fd < 0) {
		errno_t err = errno;
		LS_ERROR()
			<< "socket() failed: " << std::strerror(err)
			<< std::endl;
		return NULL;
	}
	LS_DEBUG()
		<< "ServerSocket created: fd=" << fd
		<< std::endl;
	int yes = 1;
	int ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
	if (ret < 0) {
		errno_t err = errno;
		LS_ERROR()
			<< "setsockopt() failed: " << std::strerror(err)
			<< std::endl;
		close(fd);
		return NULL;
	}
	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		errno_t err = errno;
		LS_ERROR()
			<< "bind() failed: " << std::strerror(err)
			<< std::endl;
		close(fd);
		return NULL;
	}
	LS_DEBUG()
		<< "ServerSocket bind to: " << utils::to_string(addr)
		<< std::endl;
	if (listen(fd, ACCEPT_BACKLOG) < 0) {
		errno_t err = errno;
		LS_ERROR()
			<< "listen() failed: " << std::strerror(err)
			<< std::endl;
		close(fd);
		return NULL;
	}
	LS_DEBUG()
		<< "ServerSocket listen with backlog: " << ACCEPT_BACKLOG
		<< std::endl;

	if (fcntl(fd, F_SETFD, FD_CLOEXEC) < 0) {
		LS_ERROR()
			<< "Failed to set FD_CLOEXEC"
			<< std::endl;
		close(fd);
		return NULL;
	}

	LS_INFO()
		<< "ServerSocket created: fd=" << fd
		<< ", address=" << utils::to_string(addr)
		<< ", port=" << port
		<< std::endl;

	return new ServerSocket(fd, logger, listenConfigList);
}

ServerSocket::ServerSocket(
	int fd,
	const Logger &logger,
	const ServerRunningConfigListType &listenConfigList
) : Pollable(fd),
		logger(logger, "Server=" + Pollable::getUUID().toString()),
		_listenConfigList(listenConfigList)
{
	LS_INFO()
		<< "ServerSocket initialized: fd=" << fd
		<< ", listenConfigList.size()=" << listenConfigList.size()
		<< std::endl;
}

ServerSocket::~ServerSocket()
{
	// serverFdのcloseは、Socketのデストラクタで行われる
	// clientSocketのDisposeは、socketListから削除するときに行われる
	CS_DEBUG()
		<< "ServerSocket disposed: fd=" << this->getFD()
		<< std::endl;
}

}	 // namespace webserv
