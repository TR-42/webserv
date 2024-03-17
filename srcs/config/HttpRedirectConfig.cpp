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
)
{
	*this = from;
}

HttpRedirectConfig::HttpRedirectConfig(
	const std::string &to,
	std::uint16_t code
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

	this->setProps(
		from._To,
		from._Code
	);

	return *this;
}

void webserv::HttpRedirectConfig::setProps(
	const std::string &to,
	std::uint16_t code
)
{
	this->_To = to;
	this->_Code = code;
}

webserv::HttpRedirectConfig::~HttpRedirectConfig()
{
}

IMPL_REF_GETTER_SETTER_NS(std::string, To, HttpRedirectConfig::)
IMPL_GETTER_SETTER_NS(std::uint16_t, Code, HttpRedirectConfig::)

}	 // namespace webserv
