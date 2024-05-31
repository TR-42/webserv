#include <config/HttpRedirectConfig.hpp>
namespace webserv
{

HttpRedirectConfig::HttpRedirectConfig(
) : _To(),
		_Code(0)
{
}

webserv::HttpRedirectConfig::HttpRedirectConfig(
	const HttpRedirectConfig &from
) : _To(from._To),
		_Code(from._Code)
{
}

HttpRedirectConfig::HttpRedirectConfig(
	const std::string &to,
	uint16_t code
) : _To(to),
		_Code(code)
{
}

HttpRedirectConfig &webserv::HttpRedirectConfig::operator=(
	const HttpRedirectConfig &from
)
{
	if (this == &from)
		return *this;

	this->_To = from._To;
	this->_Code = from._Code;

	return *this;
}

webserv::HttpRedirectConfig::~HttpRedirectConfig()
{
}

}	 // namespace webserv
