#include <sys/socket.h>

#include <socket/ClientSocket.hpp>
#include <utils.hpp>
#include <utils/ErrorPageProvider.hpp>
#include <utils/UUIDv7.hpp>

#define RECV_BUFFER_SIZE 1024

namespace webserv
{

SockEventResultType ClientSocket::onEventGot(
	short revents,
	std::vector<Socket *> &sockets
)
{
	(void)sockets;
	// TODO: タイムアウト監視 (呼び出し元でのreventチェックを取り除いて実装)

	if (IS_POLLIN(revents)) {
		CS_DEBUG()
			<< "POLLIN event"
			<< std::endl;
		return this->_processPollIn();
	} else if (IS_POLLOUT(revents)) {
		CS_DEBUG()
			<< "POLLOUT event"
			<< std::endl;
		return this->_processPollOut();
	} else {
		return SockEventResult::OK;
	}
}

SockEventResultType ClientSocket::_processPollIn()
{
	if (!this->httpRequest.isParseCompleted()) {
		return SockEventResult::OK;
	}

	char buffer[RECV_BUFFER_SIZE];
	ssize_t recvSize = recv(
		this->getFD(),
		buffer,
		RECV_BUFFER_SIZE,
		0
	);

	if (recvSize < 0) {
		const char *errorStr = strerror(errno);
		CS_FATAL()
			<< "recv() failed: " << errorStr
			<< std::endl;
		return SockEventResult::ERROR;
	}

	if (recvSize == 0) {
		CS_INFO()
			<< "Connection closed by peer"
			<< std::endl;
		return SockEventResult::DISPOSE_REQUEST;
	}

	bool pushResult = this->httpRequest.pushRequestRaw(std::vector<uint8_t>(buffer, buffer + recvSize));
	if (!pushResult) {
		CS_WARN()
			<< "httpRequest.pushRequestRaw() failed"
			<< std::endl;
		this->httpResponseBuffer = utils::ErrorPageProvider().badRequest().generateResponsePacket();
		return SockEventResult::OK;
	}

	if (this->httpRequest.isParseCompleted()) {
		CS_DEBUG()
			<< "Request parse completed"
			<< std::endl;
		std::string responseStr = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
		return SockEventResult::OK;
	}

	return SockEventResult::OK;
}

SockEventResultType ClientSocket::_processPollOut()
{
	if (this->httpResponseBuffer.empty()) {
		CS_DEBUG()
			<< "httpResponseBuffer is empty"
			<< std::endl;
		return SockEventResult::OK;
	}

	ssize_t sendSize = send(
		this->getFD(),
		this->httpResponseBuffer.data(),
		this->httpResponseBuffer.size(),
		0
	);

	if (sendSize < 0) {
		const char *errorStr = strerror(errno);
		CS_FATAL()
			<< "send() failed: " << errorStr
			<< std::endl;
		return SockEventResult::ERROR;
	}

	if (sendSize == 0) {
		CS_INFO()
			<< "Connection closed by peer"
			<< std::endl;
		return SockEventResult::DISPOSE_REQUEST;
	}

	this->httpResponseBuffer.erase(
		this->httpResponseBuffer.begin(),
		this->httpResponseBuffer.begin() + sendSize
	);

	if (this->httpResponseBuffer.empty()) {
		CS_DEBUG()
			<< "httpResponseBuffer is empty => response all sent"
			<< std::endl;
		return SockEventResult::DISPOSE_REQUEST;
	}

	return SockEventResult::OK;
}

ClientSocket::~ClientSocket()
{
}

ClientSocket::ClientSocket(
	int fd,
	const std::string &serverLoggerCustomId
) : Socket(fd),
		logger(serverLoggerCustomId + ",Connection=" + Socket::getUUID().toString())
{
	CS_DEBUG()
		<< "ClientSocket(fd:" << utils::to_string(fd) << ")"
		<< " created: " << this->getUUID()
		<< std::endl;
}

}	 // namespace webserv
