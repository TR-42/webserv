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

	if (this->httpRequest.isParseCompleted()) {
		CS_DEBUG()
			<< "Request parse already completed"
			<< std::endl;
		return SockEventResult::OK;
	}

	bool pushResult = this->httpRequest.pushRequestRaw(std::vector<uint8_t>(buffer, buffer + recvSize));
	if (!pushResult) {
		CS_WARN()
			<< "httpRequest.pushRequestRaw() failed"
			<< std::endl;
		this->_setResponse(utils::ErrorPageProvider().badRequest());
		return SockEventResult::OK;
	}

	if (this->httpRequest.isParseCompleted()) {
		CS_DEBUG()
			<< "Request parse completed"
			<< std::endl;
		std::string responseStr = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
		this->_setResponse(responseStr);
		return SockEventResult::OK;
	}

	C_DEBUG("processPollIn() end");

	return SockEventResult::OK;
}

SockEventResultType ClientSocket::_processPollOut()
{
	// POLLOUTの設定仕様上、this->_IsResponseSetがtrueの場合のみこの関数が呼ばれる
	if (this->httpResponseBuffer.empty()) {
		CS_DEBUG()
			<< "httpResponseBuffer is empty && can call send() => Connection can be closed"
			<< std::endl;
		return SockEventResult::DISPOSE_REQUEST;
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

	return SockEventResult::OK;
}

void ClientSocket::_setResponse(
	const std::vector<uint8_t> &response
)
{
	this->httpResponseBuffer = response;
	this->_IsResponseSet = true;
}

void ClientSocket::_setResponse(
	const std::string &responseStr
)
{
	this->_setResponse(std::vector<uint8_t>(responseStr.begin(), responseStr.end()));
}

void ClientSocket::_setResponse(
	const HttpResponse &response
)
{
	this->_setResponse(response.generateResponsePacket());
}

void ClientSocket::setToPollFd(
	struct pollfd &pollFd
) const
{
	Socket::setToPollFd(pollFd);
	pollFd.events = this->_IsResponseSet ? POLLOUT : POLLIN;
	CS_DEBUG()
		<< "POLLIN: " << IS_POLLIN(pollFd.events)
		<< ", POLLOUT: " << IS_POLLOUT(pollFd.events)
		<< std::endl;
}

ClientSocket::~ClientSocket()
{
}

ClientSocket::ClientSocket(
	int fd,
	const std::string &serverLoggerCustomId
) : Socket(fd),
		logger(serverLoggerCustomId + ", Connection=" + Socket::getUUID().toString()),
		_IsResponseSet(false)
{
	CS_DEBUG()
		<< "ClientSocket(fd:" << utils::to_string(fd) << ")"
		<< " created: " << this->getUUID()
		<< std::endl;
}

}	 // namespace webserv
