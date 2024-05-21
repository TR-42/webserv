#pragma once

#include <poll.h>

#include <vector>

#include "Logger.hpp"
#include "cgi/CgiResponseMode.hpp"
#include "http/HttpFieldMap.hpp"
#include "service/ServiceEventResult.hpp"
#include "utils.hpp"
#include "utils/ErrorPageProvider.hpp"

namespace webserv
{
class CgiResponse
{
 public:
	CgiResponse(const Logger &logger);
	~CgiResponse();
	CgiResponse(const CgiResponse &other);
	CgiResponse &operator=(const CgiResponse &other);
	std::vector<uint8_t> generateResponsePacket(bool withBody) const;
	HttpResponse getHttpResponse() const;

	CgiResponseModeType getMode() const;
	const std::string &getContentType() const;
	const std::string &getLocalLocation() const;
	const std::string &getClientLocation() const;
	const std::string &getStatusCode() const;
	const std::string &getReasonPhrase() const;
	const HttpFieldMap &getProtocolFieldMap() const;
	const HttpFieldMap &getExtensionFieldMap() const;
	const std::vector<uint8_t> &getResponseBody() const;

	bool pushResponseRaw(const std::vector<uint8_t> &responseRaw);

 private:
	Logger logger;
	CgiResponseModeType _mode;
	std::string _ContentType;
	std::string _LocalLocation;
	std::string _ClientLocation;
	std::string _StatusCode;
	std::string _ReasonPhrase;

	HttpFieldMap _ProtocolFieldMap;
	HttpFieldMap _ExtensionFieldMap;

	bool _IsResponseHeaderParsed;

	std::vector<uint8_t> _UnparsedResponseRaw;

	bool parseResponseHeader(const std::vector<uint8_t> &responseRawLine);
};

}	 // namespace webserv
