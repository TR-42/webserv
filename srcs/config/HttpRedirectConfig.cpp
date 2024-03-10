#include <config/HttpRedirectConfig.hpp>

namespace webserv
{

HttpRedirectConfig::HttpRedirectConfig(
) : _To(),
		_Code(0)
{
}

HttpRedirectConfig::HttpRedirectConfig(
	const std::string& to,
	std::uint16_t code
) : _To(to),
		_Code(code)
{
}

IMPL_REF_GETTER_SETTER_NS(std::string, To, HttpRedirectConfig::)
IMPL_GETTER_SETTER_NS(std::uint16_t, Code, HttpRedirectConfig::)

}	 // namespace webserv
