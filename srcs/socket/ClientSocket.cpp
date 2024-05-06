#include <sys/socket.h>

#include <config/ServerRunningConfig.hpp>
#include <macros.hpp>
#include <service/DeleteFileService.hpp>
#include <service/GetFileService.hpp>
#include <service/pickService.hpp>
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

	if (this->_service != NULL) {
		return this->_processPollService(revents);
	}

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

	if (!this->httpRequest.isParseCompleted()) {
		CS_DEBUG()
			<< "Request parse not completed"
			<< std::endl;
		return SockEventResult::OK;
	}

	CS_DEBUG()
		<< "Request parse completed"
		<< std::endl;
	this->_service = pickService(
		this->_listenConfigList,
		this->httpRequest,
		this->logger
	);
	if (this->_service == NULL) {
		CS_DEBUG()
			<< "pickService() returned NULL"
			<< std::endl;
		// TODO: Method Not Allowed	405
		this->_setResponse(utils::ErrorPageProvider().notImplemented());
		return SockEventResult::OK;
	}
	return this->_processPollService(0);
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

SockEventResultType ClientSocket::_processPollService(short revents)
{
	ServiceEventResultType serviceResult = this->_service->onEventGot(revents);
	switch (serviceResult) {
		case ServiceEventResult::COMPLETE:
			CS_DEBUG()
				<< "ServiceEventResult::COMPLETE"
				<< std::endl;
			this->_setResponse(this->_service->getResponse());
			delete this->_service;
			this->_service = NULL;
			return SockEventResult::OK;

		case ServiceEventResult::ERROR:
			CS_DEBUG()
				<< "ServiceEventResult::ERROR"
				<< std::endl;
			delete this->_service;
			this->_service = NULL;
			this->_setResponse(utils::ErrorPageProvider().internalServerError());
			return SockEventResult::OK;

		case ServiceEventResult::CONTINUE:
			CS_DEBUG()
				<< "ServiceEventResult::CONTINUE"
				<< std::endl;
			return SockEventResult::OK;

		default:
			CS_DEBUG()
				<< "ServiceEventResult::UNKNOWN"
				<< std::endl;
			return SockEventResult::OK;
	}
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
	if (this->_service == NULL) {
		CS_DEBUG()
			<< "ClientSocket::setToPollFd() called - this->_service == NULL"
			<< std::endl;
		Socket::setToPollFd(pollFd);
		pollFd.events = this->_IsResponseSet ? POLLOUT : POLLIN;
	} else {
		CS_DEBUG()
			<< "ClientSocket::setToPollFd() called - this->_service is set"
			<< std::endl;
		this->_service->setToPollFd(pollFd);
	}

	CS_DEBUG()
		<< "POLLIN: " << IS_POLLIN(pollFd.events)
		<< ", POLLOUT: " << IS_POLLOUT(pollFd.events)
		<< std::endl;
}

ClientSocket::~ClientSocket()
{
	CS_INFO()
		<< "ClientSocket(fd:" << this->getFD() << ")"
		<< " destroyed"
		<< std::endl;
}

ClientSocket::ClientSocket(
	int fd,
	const std::string &serverLoggerCustomId,
	const ServerRunningConfigListType &listenConfigList
) : Socket(fd),
		_listenConfigList(listenConfigList),
		logger(serverLoggerCustomId + ", Connection=" + Socket::getUUID().toString()),
		_IsResponseSet(false),
		_service(NULL)
{
	CS_DEBUG()
		<< "ClientSocket(fd:" << utils::to_string(fd) << ")"
		<< " created: " << this->getUUID()
		<< std::endl;
}

}	 // namespace webserv
