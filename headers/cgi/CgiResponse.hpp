#pragma once

#include <poll.h>

#include <vector>

#include "cgi/CgiResponseMode.hpp"
#include "http/HttpFieldMap.hpp"
#include "service/ServiceEventResult.hpp"
#include "utils/ErrorPageProvider.hpp"

namespace webserv
{
class CgiResponse
{
 public:
	CgiResponse();
	~CgiResponse();
	CgiResponse(const CgiResponse &other);
	CgiResponse &operator=(const CgiResponse &other);
	void setResponseBody(const std::string &body);
	std::vector<uint8_t> generateResponsePacket() const;

 private:
	int _fd;
	utils::ErrorPageProvider _errorPageProvider;
	CgiResponseModeType _mode;
	std::string _ContentType;
	std::string _LocalLocation;
	std::string _ClientLocation;
	std::string _StatusCode;
	std::string _ReasonPhrase;

	HttpFieldMap _ProtocolFieldMap;
	HttpFieldMap _ExtensionFieldMap;

	std::vector<uint8_t> _responseBody;
};

}	 // namespace webserv
