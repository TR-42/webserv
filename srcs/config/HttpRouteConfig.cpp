#include <config/HttpRouteConfig.hpp>

namespace webserv
{

HttpRouteConfig::HttpRouteConfig(
) : _RequestPath(),
		_Methods(),
		_Redirect(),
		_DocumentRoot(),
		_IsDocumentListingEnabled(false),
		_IndexFileList(),
		_CgiConfigList()
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
	const HttpRedirectConfig &redirect
)
{
	this->_Methods = methods;
	this->_Redirect = redirect;
}

HttpRouteConfig &webserv::HttpRouteConfig::operator=(
	const HttpRouteConfig &from
)
{
	if (this == &from)
		return *this;

	this->setProps(
		from._Methods,
		from._Redirect
	);

	return *this;
}

webserv::HttpRouteConfig::~HttpRouteConfig()
{
}

IMPL_REF_GETTER_SETTER_NS(std::string, RequestPath, HttpRouteConfig::)
IMPL_REF_GETTER_SETTER_NS(std::vector<std::string>, Methods, HttpRouteConfig::)
IMPL_REF_GETTER_SETTER_NS(HttpRedirectConfig, Redirect, HttpRouteConfig::)
IMPL_REF_GETTER_SETTER_NS(std::string, DocumentRoot, HttpRouteConfig::)
IMPL_GETTER_SETTER_NS(bool, IsDocumentListingEnabled, HttpRouteConfig::)
IMPL_REF_GETTER_SETTER_NS(std::vector<std::string>, IndexFileList, HttpRouteConfig::)
IMPL_REF_GETTER_SETTER_NS(CgiConfigListType, CgiConfigList, HttpRouteConfig::)

}	 // namespace webserv
