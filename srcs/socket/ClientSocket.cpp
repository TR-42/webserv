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

PollEventResultType ClientSocket::onEventGot(
	short revents,
	std::vector<Pollable *> &pollableList
)
{
	(void)pollableList;
	// TODO: タイムアウト監視 (呼び出し元でのreventチェックを取り除いて実装)

	if (this->_service != NULL) {
		if (this->_isServiceDisposing) {
			this->_processPollService(0);
		} else {
			return this->_processPollService(revents);
		}
	}

	if (IS_POLLIN(revents)) {
		CS_DEBUG()
			<< "POLLIN event"
			<< std::endl;
		return this->_processPollIn(pollableList);
	} else if (IS_POLLOUT(revents)) {
		CS_DEBUG()
			<< "POLLOUT event"
			<< std::endl;
		return this->_processPollOut();
	} else {
		return PollEventResult::OK;
	}
}

PollEventResultType ClientSocket::_processPollIn(
	std::vector<Pollable *> &pollableList
)
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
		return PollEventResult::ERROR;
	}

	if (recvSize == 0) {
		CS_INFO()
			<< "Connection closed by peer"
			<< std::endl;
		return PollEventResult::DISPOSE_REQUEST;
	}

	if (this->httpRequest.isParseCompleted()) {
		CS_DEBUG()
			<< "Request parse already completed"
			<< std::endl;
		return PollEventResult::OK;
	}

	bool pushResult = this->httpRequest.pushRequestRaw(std::vector<uint8_t>(buffer, buffer + recvSize));
	if (!pushResult) {
		CS_WARN()
			<< "httpRequest.pushRequestRaw() failed"
			<< std::endl;
		this->_setResponse(utils::ErrorPageProvider().badRequest());
		return PollEventResult::OK;
	}

	if (!this->httpRequest.isParseCompleted()) {
		CS_DEBUG()
			<< "Request parse not completed"
			<< std::endl;
		return PollEventResult::OK;
	}

	CS_DEBUG()
		<< "Request parse completed"
		<< std::endl;
	this->_service = pickService(
		this->_listenConfigList,
		this->httpRequest,
		pollableList,
		this->logger
	);
	if (this->_service == NULL) {
		CS_DEBUG()
			<< "pickService() returned NULL"
			<< std::endl;
		// TODO: Method Not Allowed	405
		this->_setResponse(utils::ErrorPageProvider().notImplemented());
		return PollEventResult::OK;
	}
	return this->_processPollService(0);
}

PollEventResultType ClientSocket::_processPollOut()
{
	// POLLOUTの設定仕様上、this->_IsResponseSetがtrueの場合のみこの関数が呼ばれる
	if (this->httpResponseBuffer.empty()) {
		CS_DEBUG()
			<< "httpResponseBuffer is empty && can call send() => Connection can be closed"
			<< std::endl;
		return PollEventResult::DISPOSE_REQUEST;
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
		return PollEventResult::ERROR;
	}

	if (sendSize == 0) {
		CS_INFO()
			<< "Connection closed by peer"
			<< std::endl;
		return PollEventResult::DISPOSE_REQUEST;
	}

	this->httpResponseBuffer.erase(
		this->httpResponseBuffer.begin(),
		this->httpResponseBuffer.begin() + sendSize
	);

	return PollEventResult::OK;
}

PollEventResultType ClientSocket::_processPollService(short revents)
{
	ServiceEventResultType serviceResult = this->_service->onEventGot(revents);
	switch (serviceResult) {
		case ServiceEventResult::COMPLETE:
			CS_DEBUG()
				<< "ServiceEventResult::COMPLETE"
				<< std::endl;
			this->_setResponse(this->_service->getResponse());
			if (this->_service->canDispose()) {
				delete this->_service;
				this->_service = NULL;
			} else {
				this->_isServiceDisposing = true;
			}
			return PollEventResult::OK;

		case ServiceEventResult::ERROR:
			CS_DEBUG()
				<< "ServiceEventResult::ERROR"
				<< std::endl;
			delete this->_service;
			this->_service = NULL;
			if (this->_isServiceDisposing) {
				C_ERROR("ServiceEventResult::ERROR && this->_isServiceDisposing == true");
			} else {
				this->_setResponse(utils::ErrorPageProvider().internalServerError());
			}
			return PollEventResult::OK;

		case ServiceEventResult::CONTINUE:
			CS_DEBUG()
				<< "ServiceEventResult::CONTINUE"
				<< std::endl;
			return PollEventResult::OK;

		default:
			CS_DEBUG()
				<< "ServiceEventResult::UNKNOWN"
				<< std::endl;
			return PollEventResult::OK;
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
	if (this->_service == NULL || this->_isServiceDisposing) {
		CS_DEBUG()
			<< "ClientSocket::setToPollFd() called - this->_service == NULL"
			<< std::endl;
		Pollable::setToPollFd(pollFd);
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
	const ServerRunningConfigListType &listenConfigList,
	const Logger &logger
) : Pollable(fd),
		_listenConfigList(listenConfigList),
		logger(logger, serverLoggerCustomId + ", Connection=" + Pollable::getUUID().toString()),
		httpRequest(this->logger),
		_IsResponseSet(false),
		_service(NULL),
		_isServiceDisposing(false)
{
	CS_DEBUG()
		<< "ClientSocket(fd:" << utils::to_string(fd) << ")"
		<< " created: " << this->getUUID()
		<< std::endl;
}

}	 // namespace webserv
