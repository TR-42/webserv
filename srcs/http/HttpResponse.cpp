#include <http/HttpResponse.hpp>

namespace webserv
{

IMPL_GETTER_SETTER_NS(std::string, Version, HttpResponse::)
IMPL_GETTER_SETTER_NS(std::string, StatusCode, HttpResponse::)
IMPL_GETTER_SETTER_NS(std::string, ReasonPhrase, HttpResponse::)
IMPL_REF_NO_CONST_GETTER_SETTER_NS(ResponseHeaderMap, Headers, HttpResponse::)
IMPL_REF_NO_CONST_GETTER_SETTER_NS(std::vector<uint8_t>, Body, HttpResponse::)

std::vector<uint8_t> HttpResponse::generateResponsePacket() const
{
	std::vector<uint8_t> responsePacket;
	responsePacket.reserve(1024);

	responsePacket.insert(responsePacket.end(), this->_Version.begin(), this->_Version.end());
	responsePacket.push_back(' ');
	responsePacket.insert(responsePacket.end(), this->_StatusCode.begin(), this->_StatusCode.end());
	responsePacket.push_back(' ');
	responsePacket.insert(responsePacket.end(), this->_ReasonPhrase.begin(), this->_ReasonPhrase.end());
	responsePacket.push_back('\r');
	responsePacket.push_back('\n');

	for (ResponseHeaderMap::const_iterator it = this->_Headers.begin(); it != this->_Headers.end(); ++it) {
		responsePacket.insert(responsePacket.end(), it->first.begin(), it->first.end());
		responsePacket.push_back(':');
		responsePacket.push_back(' ');
		// fixme: set-cookie headerは個別に設定する必要がある

		std::vector<std::string>::const_iterator it2_begin = it->second.begin();
		for (std::vector<std::string>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
			if (it2 != it2_begin) {
				responsePacket.push_back(',');
			}
			responsePacket.insert(responsePacket.end(), it2->begin(), it2->end());
		}
		responsePacket.push_back('\r');
		responsePacket.push_back('\n');
	}

	responsePacket.push_back('\r');
	responsePacket.push_back('\n');

	responsePacket.insert(responsePacket.end(), this->_Body.begin(), this->_Body.end());

	return responsePacket;
}

}	 // namespace webserv
