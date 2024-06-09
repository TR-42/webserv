#pragma once

#include <poll.h>

#include <http/MessageBody.hpp>
#include <types.hpp>
#include <vector>

#include "Logger.hpp"
#include "cgi/CgiResponseMode.hpp"
#include "http/HttpFieldMap.hpp"
#include "service/ServiceEventResult.hpp"
#include "utils/ErrorPageProvider.hpp"

namespace webserv
{
class CgiResponse
{
 public:
	CgiResponse(const Logger &logger, const utils::ErrorPageProvider &errorPageProvider);
	~CgiResponse();
	CgiResponse(const CgiResponse &other);
	std::vector<uint8_t> generateResponsePacket(bool withBody) const;
	HttpResponse getHttpResponse() const;

	CgiResponseModeType getMode() const;
	const std::string &getContentType() const;
	const std::string &getLocation() const;
	const std::string &getStatusCode() const;
	const std::string &getReasonPhrase() const;
	const HttpFieldMap &getProtocolFieldMap() const;
	const HttpFieldMap &getExtensionFieldMap() const;
	const MessageBody &getResponseBody() const;

	bool pushResponseRaw(const std::vector<uint8_t> &responseRaw);
	bool pushResponseRaw(const uint8_t *responseRaw, size_t responseRawSize);

 private:
	Logger logger;
	CgiResponseModeType _mode;
	std::string _ContentType;
	std::string _Location;
	std::string _StatusCode;
	std::string _ReasonPhrase;

	HttpFieldMap _ProtocolFieldMap;
	HttpFieldMap _ExtensionFieldMap;

	bool _IsResponseHeaderParsed;

	bool isSetContentType;
	bool isSetLocation;
	bool isSetStatus;
	bool isSetProtocolFieldMap;
	bool isSetExtensionFieldMap;
	bool isSetResponseBody;
	bool isAbsolutePath;

	std::vector<uint8_t> _UnparsedResponseRaw;
	MessageBody _ResponseBody;

	utils::ErrorPageProvider _errorPageProvider;

	bool parseResponseHeader(const std::vector<uint8_t> &responseRawLine);

	CgiResponse &operator=(const CgiResponse &other);
};

}	 // namespace webserv
