#include <fcntl.h>
#include <sys/socket.h>

#include <algorithm>
#include <cerrno>
#include <climits>
#include <config/ServerRunningConfig.hpp>
#include <cstring>
#include <macros.hpp>
#include <service/CgiService.hpp>
#include <service/DeleteFileService.hpp>
#include <service/GetFileService.hpp>
#include <service/RequestedFileInfo.hpp>
#include <service/pickService.hpp>
#include <socket/ClientSocket.hpp>
#include <utils/ErrorPageProvider.hpp>
#include <utils/UUIDv7.hpp>
#include <utils/to_string.hpp>

#define RECV_BUFFER_SIZE (256 * 256 * 256)

namespace webserv
{

PollEventResultType ClientSocket::onEventGot(
	int fd,
	short revents,
	std::vector<Pollable *> &pollableList,
	const struct timespec &now
)
{
	// 送信しきった後、ソケットがcloseされた場合はcloseする
	if (this->_IsResponseSet && this->httpResponseBuffer.size() == this->_responseBufferOffset && IS_POLLOUT(revents)) {
		CS_INFO()
			<< "Response buffer is empty(= EOF sent) && POLLOUT -> dispose"
			<< std::endl;
		return PollEventResult::DISPOSE_REQUEST;
	}

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
		PollEventResultType result = this->_processPollIn(now, pollableList);
		if (result != PollEventResult::OK) {
			return result;
		}
		// サービス終了後に (POLLOUTが設定されていて) すぐに返せる場合のため、POLLOUTのチェックも行う
	}
	if (this->_IsResponseSet && IS_POLLOUT(revents)) {
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
	if (this->_readBuf == NULL) {
		try {
			this->_readBuf = new uint8_t[RECV_BUFFER_SIZE];
		} catch (const std::exception &e) {
			CS_FATAL()
				<< "Failed to allocate memory for read buffer: " << e.what()
				<< std::endl;
			return PollEventResult::ERROR;
		}
	}

	ssize_t recvSize = recv(
		this->getFD(),
		this->_readBuf,
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

	CS_LOG()
		<< "Received " << recvSize << " bytes"
		<< "(total: " << this->httpRequest.getTotalRequestSize() + recvSize << " bytes)"
		<< std::endl;

	if (this->httpRequest.isParseCompleted()) {
		CS_DEBUG()
			<< "Request parse already completed"
			<< std::endl;
		return PollEventResult::OK;
	}

	try {
		bool pushResult = this->httpRequest.pushRequestRaw(this->_readBuf, recvSize);
		if (!pushResult) {
			CS_WARN()
				<< "httpRequest.pushRequestRaw() failed"
				<< std::endl;
			if (this->httpRequest.isServerRunningConfigSet()) {
				this->_setResponse(this->httpRequest.getServerRunningConfig().getErrorPageProvider().badRequest());
			} else {
				this->_setResponse(this->_listenConfigList[0].getErrorPageProvider().badRequest());
			}
			return PollEventResult::OK;
		}
	} catch (http::exception::HttpError &e) {
		CS_WARN()
			<< "httpRequest.pushRequestRaw() failed(HTTPError): " << e.what()
			<< std::endl;
		if (this->httpRequest.isServerRunningConfigSet()) {
			this->_setResponse(e.toResponse(this->httpRequest.getServerRunningConfig().getErrorPageProvider(), this->logger));
		} else {
			this->_setResponse(e.toResponse(this->_listenConfigList[0].getErrorPageProvider(), this->logger));
		}
		return PollEventResult::OK;
	} catch (const std::exception &e) {
		CS_WARN()
			<< "httpRequest.pushRequestRaw() failed(Exception): " << e.what()
			<< std::endl;
		if (this->httpRequest.isServerRunningConfigSet()) {
			this->_setResponse(this->httpRequest.getServerRunningConfig().getErrorPageProvider().internalServerError());
		} else {
			this->_setResponse(this->_listenConfigList[0].getErrorPageProvider().internalServerError());
		}
		return PollEventResult::OK;
	}

	if (!this->_IsHeaderValidationCompleted && this->httpRequest.isRequestHeaderParsed()) {
		this->_IsHeaderValidationCompleted = true;

		httpRequest.setServerRunningConfig(this->_listenConfigList);
		const ServerRunningConfig &serverRunningConfig = this->httpRequest.getServerRunningConfig();

		// chunkedの場合も一旦チェック
		if (this->httpRequest.isSizeLimitExceeded()) {
			CS_WARN()
				<< "Request size limit exceeded"
				<< std::endl;
			this->_setResponse(serverRunningConfig.getErrorPageProvider().requestEntityTooLarge());
			return PollEventResult::OK;
		}

		CS_DEBUG()
			<< "Request Body Limit(Server): " << serverRunningConfig.getRequestBodyLimit()
			<< ", Request Body Limit(Route): " << this->httpRequest.getRouteConfig().getRequestBodyLimit()
			<< std::endl;

		this->_timeoutChecker.setTimeoutMs(serverRunningConfig.getTimeoutMs());
		if (this->_timeoutChecker.isTimeouted(now)) {
			CS_WARN()
				<< "Request timeout"
				<< std::endl;
			this->_setResponse(serverRunningConfig.getErrorPageProvider().requestTimeout());
			return PollEventResult::OK;
		}

		const std::set<std::string> &allowedMethods = this->httpRequest.getRouteConfig().getMethods();
		if (!allowedMethods.empty() && allowedMethods.find(this->httpRequest.getMethod()) == allowedMethods.end()) {
			CS_WARN()
				<< "Method not allowed"
				<< std::endl;
			this->_setResponse(serverRunningConfig.getErrorPageProvider().methodNotAllowed());
			return PollEventResult::OK;
		}

		const HttpRedirectConfig &redirect = this->httpRequest.getRouteConfig().getRedirect();
		if (!redirect.getTo().empty()) {
			ServerRunningConfig serverRunningConfig = this->httpRequest.getServerRunningConfig();
			utils::ErrorPageProvider errorPageProvider = serverRunningConfig.getErrorPageProvider();
			HttpResponse response = errorPageProvider.getErrorPage(redirect.getCode());
			response.getHeaders().addValue("Location", redirect.getTo());
			this->_setResponse(response);
			return PollEventResult::OK;
		}
	} else if (this->httpRequest.isRequestHeaderParsed() && this->httpRequest.getBody().getIsChunked()) {
		// ここに来る時点で、セット済みであるはずである
		const ServerRunningConfig &serverRunningConfig = this->httpRequest.getServerRunningConfig();
		if (this->httpRequest.isSizeLimitExceeded()) {
			CS_WARN()
				<< "Request size limit exceeded"
				<< std::endl;
			this->_setResponse(serverRunningConfig.getErrorPageProvider().requestEntityTooLarge());
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
		<< "Body size: " << this->httpRequest.getBody().size()
		<< " (chunked: " << std::boolalpha << this->httpRequest.getBody().getIsChunked() << ")"
		<< std::endl;

	delete[] this->_readBuf;
	this->_readBuf = NULL;

	try {
		this->_service = pickService(
			this->httpRequest.getServerRunningConfig().getPort(),
			this->httpRequest.getRouteConfig(),
			this->_clientAddr,
			this->httpRequest,
			pollableList,
			this->logger
		);
	} catch (http::exception::HttpError &e) {
		CS_WARN()
			<< "pickService() failed(HTTPError): " << e.what()
			<< std::endl;
		if (this->httpRequest.isServerRunningConfigSet()) {
			this->_setResponse(e.toResponse(this->httpRequest.getServerRunningConfig().getErrorPageProvider(), this->logger));
		} else {
			this->_setResponse(e.toResponse(this->_listenConfigList[0].getErrorPageProvider(), this->logger));
		}
		return PollEventResult::OK;
	} catch (const std::exception &e) {
		CS_WARN()
			<< "pickService() failed(Exception): " << e.what()
			<< std::endl;
		if (this->httpRequest.isServerRunningConfigSet()) {
			this->_setResponse(this->httpRequest.getServerRunningConfig().getErrorPageProvider().internalServerError());
		} else {
			this->_setResponse(this->_listenConfigList[0].getErrorPageProvider().internalServerError());
		}
		return PollEventResult::OK;
	}

	if (this->_service == NULL) {
		CS_DEBUG()
			<< "pickService() returned NULL"
			<< std::endl;
		if (this->httpRequest.isServerRunningConfigSet()) {
			this->_setResponse(this->httpRequest.getServerRunningConfig().getErrorPageProvider().internalServerError());
		} else {
			this->_setResponse(this->_listenConfigList[0].getErrorPageProvider().internalServerError());
		}
		return PollEventResult::OK;
	}

	this->_processPollService(0, pollableList);
	return PollEventResult::OK;
}

PollEventResultType ClientSocket::_processPollOut()
{
	// POLLOUTの設定仕様上、this->_IsResponseSetがtrueの場合のみこの関数が呼ばれる
	if (this->httpResponseBuffer.size() == this->_responseBufferOffset) {
		CS_DEBUG()
			<< "httpResponseBuffer is empty && can call send() => waiting for connection closing"
			<< std::endl;
		ssize_t sendSize = send(
			this->getFD(),
			NULL,
			0,
			MSG_EOF
		);
		if (sendSize < 0) {
			errno_t err = errno;
			CS_FATAL()
				<< "send() failed: " << std::strerror(err)
				<< std::endl;
			return PollEventResult::ERROR;
		}
		return PollEventResult::OK;
	}

	size_t remainSize = this->httpResponseBuffer.size() - this->_responseBufferOffset;
	if (INT_MAX < remainSize) {
		remainSize = INT_MAX;
	}

	ssize_t sendSize = send(
		this->getFD(),
		this->httpResponseBuffer.data() + this->_responseBufferOffset,
		remainSize,
		0
	);

	if (sendSize < 0) {
		errno_t err = errno;
		CS_FATAL()
			<< "send() failed: " << std::strerror(err)
			<< std::endl;
		return PollEventResult::ERROR;
	}

	if (sendSize == 0) {
		CS_INFO()
			<< "Connection closed by peer"
			<< std::endl;
		return PollEventResult::DISPOSE_REQUEST;
	}

	this->_responseBufferOffset += sendSize;

	return PollEventResult::OK;
}

void ClientSocket::_processPollService(
	short revents,
	std::vector<Pollable *> &pollableList
)
{
	ServiceEventResultType serviceResult;
	try {
		serviceResult = this->_service->onEventGot(revents);
	} catch (http::exception::HttpError &e) {
		CS_WARN()
			<< "service.onEventGot() failed(HTTPError): " << e.what()
			<< std::endl;
		if (this->httpRequest.isServerRunningConfigSet()) {
			this->_setResponse(e.toResponse(this->httpRequest.getServerRunningConfig().getErrorPageProvider(), this->logger));
		} else {
			this->_setResponse(e.toResponse(this->_listenConfigList[0].getErrorPageProvider(), this->logger));
		}
		return;
	} catch (const std::exception &e) {
		CS_WARN()
			<< "service.onEventGot() failed(Exception): " << e.what()
			<< std::endl;
		if (this->httpRequest.isServerRunningConfigSet()) {
			this->_setResponse(this->httpRequest.getServerRunningConfig().getErrorPageProvider().internalServerError());
		} else {
			this->_setResponse(this->_listenConfigList[0].getErrorPageProvider().internalServerError());
		}
		return;
	}

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
					this->httpRequest.updatePath(cgiService->getLocalRedirectLocation());
					delete this->_service;
					this->_service = NULL;

					try {
						this->_service = pickService(
							this->httpRequest.getServerRunningConfig().getPort(),
							this->httpRequest.getServerRunningConfig().pickRouteConfig(
								this->httpRequest.getPathSegmentList(),
								this->httpRequest.getMethod()
							),
							this->_clientAddr,
							this->httpRequest,
							pollableList,
							this->logger
						);
					} catch (http::exception::HttpError &e) {
						CS_WARN()
							<< "pickService() failed(HTTPError): " << e.what()
							<< std::endl;
						if (this->httpRequest.isServerRunningConfigSet()) {
							this->_setResponse(e.toResponse(this->httpRequest.getServerRunningConfig().getErrorPageProvider(), this->logger));
						} else {
							this->_setResponse(e.toResponse(this->_listenConfigList[0].getErrorPageProvider(), this->logger));
						}
						return;
					} catch (const std::exception &e) {
						CS_WARN()
							<< "pickService() failed(Exception): " << e.what()
							<< std::endl;
						if (this->httpRequest.isServerRunningConfigSet()) {
							this->_setResponse(this->httpRequest.getServerRunningConfig().getErrorPageProvider().internalServerError());
						} else {
							this->_setResponse(this->_listenConfigList[0].getErrorPageProvider().internalServerError());
						}
						return;
					}

					if (this->_service == NULL) {
						CS_DEBUG()
							<< "pickService() returned NULL"
							<< std::endl;
						if (this->httpRequest.isServerRunningConfigSet()) {
							this->_setResponse(this->httpRequest.getServerRunningConfig().getErrorPageProvider().methodNotAllowed());
						} else {
							this->_setResponse(this->_listenConfigList[0].getErrorPageProvider().methodNotAllowed());
						}
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
			if (this->httpRequest.isServerRunningConfigSet()) {
				this->_setResponse(this->httpRequest.getServerRunningConfig().getErrorPageProvider().internalServerError());
			} else {
				this->_setResponse(this->_listenConfigList[0].getErrorPageProvider().internalServerError());
			}
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
		// 初回実行でも書き込みを行うため、POLLOUTを設定する
		pollFd.events = isResponseAvailable ? POLLOUT : (POLLIN | POLLOUT);
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

	if (this->_readBuf != NULL) {
		delete[] this->_readBuf;
		this->_readBuf = NULL;
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
		_readBuf(NULL),
		httpRequest(this->logger),
		_responseBufferOffset(0),
		_IsResponseSet(false),
		_service(NULL),
		_clientAddr(clientAddr),
		_IsHeaderValidationCompleted(false),
		_timeoutChecker(now, logger)
{
	if (fcntl(fd, F_SETFL, O_NONBLOCK | O_CLOEXEC) < 0) {
		LS_ERROR()
			<< "fcntl() failed to set flags: " << std::strerror(errno)
			<< std::endl;
	}

	CS_DEBUG()
		<< "ClientSocket(fd:" << utils::to_string(fd) << ")"
		<< " created: " << this->getUUID()
		<< std::endl;
}

}	 // namespace webserv
