#include <sys/socket.h>

#include <cerrno>
#include <config/ServerRunningConfig.hpp>
#include <cstring>
#include <macros.hpp>
#include <service/CgiService.hpp>
#include <service/DeleteFileService.hpp>
#include <service/GetFileService.hpp>
#include <service/pickService.hpp>
#include <socket/ClientSocket.hpp>
#include <utils/ErrorPageProvider.hpp>
#include <utils/UUIDv7.hpp>
#include <utils/to_string.hpp>

#define RECV_BUFFER_SIZE 1024
#define WEBSERV_HTTP_REQUEST_BODY_SIZE_MAX_BYTES (128 * 1024 * 1024)

namespace webserv
{

static const ServerRunningConfig &pickServerConfig(
	const ServerRunningConfigListType &listenConfigList,
	const HttpRequest &request,
	const Logger &logger
);

PollEventResultType ClientSocket::onEventGot(
	int fd,
	short revents,
	std::vector<Pollable *> &pollableList,
	const struct timespec &now
)
{
	if (this->_timeoutChecker.isConnectionTimeouted(now)) {
		CS_WARN()
			<< "Connection Timeout"
			<< std::endl;
		return PollEventResult::DISPOSE_REQUEST;
	}

	if (!this->_IsResponseSet && this->_timeoutChecker.isTimeouted(now)) {
		if (this->httpRequest.isServerRunningConfigSet()) {
			C_WARN("Gateway Timeout");
			if (this->httpRequest.isParseCompleted()) {
				this->_setResponse(this->httpRequest.getServerRunningConfig().getErrorPageProvider().gatewayTimeout());
			} else {
				this->_setResponse(this->httpRequest.getServerRunningConfig().getErrorPageProvider().requestTimeout());
			}
		} else {
			C_WARN("Request Timeout");
			this->_setResponse(this->_listenConfigList[0].getErrorPageProvider().requestTimeout());
		}

		if (this->_service != NULL) {
			this->_service->setIsDisposingFromChildProcess(this->isDisposingFromChildProcess());
			delete this->_service;
			this->_service = NULL;
		}
	}

	if (this->_service != NULL) {
		if (this->isFdSame(fd)) {
			this->_processPollService(0, pollableList);
		} else {
			this->_processPollService(revents, pollableList);
			return PollEventResult::OK;
		}
	}

	if (IS_POLL_ANY_ERROR(revents)) {
		// エラーの場合はレスポンスを返すことができないため、サービスを終了する
		CS_ERROR()
			<< "Error event"
			<< std::endl;
		return PollEventResult::DISPOSE_REQUEST;
	}

	if (IS_POLLIN(revents)) {
		CS_DEBUG()
			<< "POLLIN event"
			<< std::endl;
		return this->_processPollIn(now, pollableList);
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
	const struct timespec &now,
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
		const char *errorStr = std::strerror(errno);
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

	try {
		bool pushResult = this->httpRequest.pushRequestRaw(std::vector<uint8_t>(buffer, buffer + recvSize));
		if (!pushResult) {
			CS_WARN()
				<< "httpRequest.pushRequestRaw() failed"
				<< std::endl;
			this->_setResponse(utils::ErrorPageProvider().badRequest());
			return PollEventResult::OK;
		}
	} catch (http::exception::HttpError &e) {
		CS_WARN()
			<< "httpRequest.pushRequestRaw() failed: " << e.what()
			<< std::endl;
		// TODO: 適切なErrorPageProviderを選択する
		this->_setResponse(e.toResponse(this->_listenConfigList[0].getErrorPageProvider(), this->logger));
		return PollEventResult::OK;
	} catch (const std::exception &e) {
		CS_WARN()
			<< "httpRequest.pushRequestRaw() failed: " << e.what()
			<< std::endl;
		// TODO: 適切なErrorPageProviderを選択する
		this->_setResponse(this->_listenConfigList[0].getErrorPageProvider().internalServerError());
		return PollEventResult::OK;
	}

	if (!this->_IsHeaderValidationCompleted && this->httpRequest.isRequestHeaderParsed()) {
		this->_IsHeaderValidationCompleted = true;

		const ServerRunningConfig &serverRunningConfig = pickServerConfig(
			this->_listenConfigList,
			this->httpRequest,
			this->logger
		);

		if (serverRunningConfig.isSizeLimitExceeded(this->httpRequest.getContentLength())) {
			CS_WARN()
				<< "Request size limit exceeded"
				<< std::endl;
			this->_setResponse(serverRunningConfig.getErrorPageProvider().requestEntityTooLarge());
			return PollEventResult::OK;
		}

		if (WEBSERV_HTTP_REQUEST_BODY_SIZE_MAX_BYTES < this->httpRequest.getContentLength()) {
			CS_WARN()
				<< "Request body size is too large"
				<< std::endl;
			this->_setResponse(serverRunningConfig.getErrorPageProvider().requestEntityTooLarge());
			return PollEventResult::OK;
		}

		httpRequest.setServerRunningConfig(serverRunningConfig);

		this->_timeoutChecker.setTimeoutMs(serverRunningConfig.getTimeoutMs());
		if (this->_timeoutChecker.isTimeouted(now)) {
			CS_WARN()
				<< "Request timeout"
				<< std::endl;
			this->_setResponse(serverRunningConfig.getErrorPageProvider().requestTimeout());
			return PollEventResult::OK;
		}
	}

	if (!this->httpRequest.isParseCompleted()) {
		CS_DEBUG()
			<< "Request parse not completed"
			<< std::endl;
		return PollEventResult::OK;
	}

	CS_DEBUG()
		<< "Request parse completed"
		<< "Body size: " << this->httpRequest.getContentLength()
		<< std::endl;
	this->_service = pickService(
		this->_clientAddr,
		this->httpRequest,
		pollableList,
		this->logger
	);
	if (this->_service == NULL) {
		CS_DEBUG()
			<< "pickService() returned NULL"
			<< std::endl;
		this->_setResponse(utils::ErrorPageProvider().methodNotAllowed());
		return PollEventResult::OK;
	}

	this->_processPollService(0, pollableList);
	return PollEventResult::OK;
}

static const ServerRunningConfig &pickServerConfig(
	const ServerRunningConfigListType &listenConfigList,
	const HttpRequest &request,
	const Logger &logger
)
{
	if (listenConfigList.empty()) {
		L_FATAL("No ServerConfig found");
		throw std::runtime_error("No ServerConfig found");
	}

	if (request.getHost().empty()) {
		return listenConfigList[0];
	}

	for (
		ServerRunningConfigListType::const_iterator itConfig = listenConfigList.begin();
		itConfig != listenConfigList.end();
		++itConfig
	) {
		if (itConfig->isServerNameMatch(request.getHost())) {
			return *itConfig;
		}
	}

	// Hostが一致するServerConfigが見つからなかった場合、一番最初に記述されていた設定に従う
	return listenConfigList[0];
};

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
		const char *errorStr = std::strerror(errno);
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

void ClientSocket::_processPollService(
	short revents,
	std::vector<Pollable *> &pollableList
)
{
	ServiceEventResultType serviceResult = this->_service->onEventGot(revents);
	switch (serviceResult) {
		case ServiceEventResult::COMPLETE:
			CS_DEBUG()
				<< "ServiceEventResult::COMPLETE"
				<< std::endl;
			if (!this->_IsResponseSet) {
				CgiService *cgiService = dynamic_cast<CgiService *>(this->_service);
				if (cgiService != NULL && cgiService->isLocalRedirect()) {
					CS_DEBUG()
						<< "Local redirect: " << cgiService->getLocalRedirectLocation()
						<< std::endl;
					this->httpRequest.setPath(cgiService->getLocalRedirectLocation());
					delete this->_service;
					this->_service = NULL;
					this->_service = pickService(
						this->_clientAddr,
						this->httpRequest,
						pollableList,
						this->logger
					);
					if (this->_service == NULL) {
						CS_DEBUG()
							<< "pickService() returned NULL"
							<< std::endl;
						this->_setResponse(utils::ErrorPageProvider().methodNotAllowed());
					} else {
						this->_processPollService(0, pollableList);
					}
					return;
				} else {
					this->_setResponse(this->_service->getResponse());
				}
			}
			delete this->_service;
			this->_service = NULL;
			return;

		case ServiceEventResult::RESPONSE_READY:
			CS_DEBUG()
				<< "ServiceEventResult::RESPONSE_READY"
				<< std::endl;
			if (!this->_IsResponseSet) {
				this->_setResponse(this->_service->getResponse());
			}
			return;

		case ServiceEventResult::ERROR:
			CS_DEBUG()
				<< "ServiceEventResult::ERROR"
				<< std::endl;
			this->_service->setIsDisposingFromChildProcess(this->isDisposingFromChildProcess());
			delete this->_service;
			this->_service = NULL;
			this->_setResponse(utils::ErrorPageProvider().internalServerError());
			return;

		case ServiceEventResult::CONTINUE:
			CS_DEBUG()
				<< "ServiceEventResult::CONTINUE"
				<< std::endl;
			return;

		default:
			CS_DEBUG()
				<< "ServiceEventResult::UNKNOWN"
				<< std::endl;
			return;
	}
}

void ClientSocket::_setResponse(
	const std::vector<uint8_t> &response
)
{
	if (this->_IsResponseSet) {
		CS_WARN()
			<< "Response is already set"
			<< std::endl;
	} else {
		this->httpResponseBuffer = response;
		CS_DEBUG()
			<< "Response set (size: " << this->httpResponseBuffer.size() << ")"
			<< std::endl;
		this->_IsResponseSet = true;
	}
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
	this->_setResponse(response.generateResponsePacket(
		this->httpRequest.getMethod() != "HEAD",
		this->httpRequest.getVersion() < HttpVersion(1, 0)
	));
}

void ClientSocket::setToPollFd(
	struct pollfd &pollFd,
	const struct timespec &now
) const
{
	Pollable::setToPollFd(pollFd, now);
	bool isResponseAvailable = this->_IsResponseSet || this->_timeoutChecker.isTimeouted(now);

	if (isResponseAvailable || this->_service == NULL) {
		pollFd.events = isResponseAvailable ? POLLOUT : POLLIN;
	} else {
		this->_service->setToPollFd(pollFd);
	}
}

ClientSocket::~ClientSocket()
{
	if (this->_service != NULL) {
		C_ERROR("Service is not disposed -> disposing...");
		this->_service->setIsDisposingFromChildProcess(this->isDisposingFromChildProcess());
		delete this->_service;
		this->_service = NULL;
	}

	CS_INFO()
		<< "ClientSocket(fd:" << this->getFD() << ")"
		<< " destroying"
		<< std::endl;

#if DEBUG
	if (!this->isDisposingFromChildProcess() && 0 <= this->getFD()) {
		CS_DEBUG()
			<< "Shutdown socket: " << this->getFD()
			<< std::endl;
		shutdown(this->getFD(), SHUT_RDWR);
	}
#endif
}

ClientSocket::ClientSocket(
	int fd,
	const struct sockaddr &clientAddr,
	const timespec &now,
	const ServerRunningConfigListType &listenConfigList,
	const Logger &logger
) : Pollable(fd),
		_listenConfigList(listenConfigList),
		logger(logger, logger.getCustomId() + ", Connection=" + Pollable::getUUID().toString()),
		httpRequest(this->logger),
		_IsResponseSet(false),
		_service(NULL),
		_clientAddr(clientAddr),
		_IsHeaderValidationCompleted(false),
		_timeoutChecker(now, logger)
{
	CS_DEBUG()
		<< "ClientSocket(fd:" << utils::to_string(fd) << ")"
		<< " created: " << this->getUUID()
		<< std::endl;
}

}	 // namespace webserv
