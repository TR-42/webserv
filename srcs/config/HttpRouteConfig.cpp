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
	const std::string &requestPath,
	const std::vector<std::string> &methods,
	const HttpRedirectConfig &redirect,
	const std::string &documentRoot,
	bool isDocumentListingEnabled,
	const std::vector<std::string> &indexFileList,
	const CgiConfigListType &cgiConfigList
)
{
	this->_RequestPath = requestPath;
	this->_Methods = methods;
	this->_Redirect = redirect;
	this->_DocumentRoot = documentRoot;
	this->_IsDocumentListingEnabled = isDocumentListingEnabled;
	this->_IndexFileList = indexFileList;
	this->_CgiConfigList = cgiConfigList;
}

HttpRouteConfig &webserv::HttpRouteConfig::operator=(
	const HttpRouteConfig &from
)
{
	if (this == &from)
		return *this;

	this->setProps(
		from._RequestPath,
		from._Methods,
		from._Redirect,
		from._DocumentRoot,
		from._IsDocumentListingEnabled,
		from._IndexFileList,
		from._CgiConfigList
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
