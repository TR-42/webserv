#include <config/HttpRouteConfig.hpp>

namespace webserv
{

HttpRouteConfig::HttpRouteConfig(
) : _Methods(),
		_Redirect()
{
}

IMPL_REF_GETTER_SETTER_NS(std::vector<std::string>, Methods, HttpRouteConfig::)
IMPL_REF_GETTER_SETTER_NS(HttpRedirectConfig, Redirect, HttpRouteConfig::)

}	 // namespace webserv
