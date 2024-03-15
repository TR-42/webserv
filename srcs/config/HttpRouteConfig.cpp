#include <config/HttpRouteConfig.hpp>

namespace webserv
{

HttpRouteConfig::HttpRouteConfig(
) : _Methods(),
		_Redirect()
{
}

webserv::HttpRouteConfig::HttpRouteConfig(
	const HttpRouteConfig &from
)
{
	*this = from;
}

void HttpRouteConfig::setProps(
	const std::vector<std::string> &methods,
	const IHttpRedirectConfig &redirect
)
{
	this->_Methods = methods;
	this->_Redirect = redirect.clone();
}

HttpRouteConfig &webserv::HttpRouteConfig::operator=(
	const HttpRouteConfig &from
)
{
	if (this == &from)
		return *this;

	this->setProps(
		from._Methods,
		*from._Redirect
	);

	return *this;
}

webserv::HttpRouteConfig::~HttpRouteConfig()
{
	delete this->_Redirect;
	this->_Redirect = NULL;
}

IHttpRouteConfig *webserv::HttpRouteConfig::clone() const
{
	return new HttpRouteConfig(*this);
}

IMPL_REF_GETTER_SETTER_NS(std::vector<std::string>, Methods, HttpRouteConfig::)
IMPL_PTR_GETTER_SETTER_NS(IHttpRedirectConfig, Redirect, HttpRouteConfig::)

}	 // namespace webserv
