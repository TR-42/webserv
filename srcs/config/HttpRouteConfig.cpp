#include <config/HttpRouteConfig.hpp>
#include <utils/split.hpp>

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

HttpRouteConfig::HttpRouteConfig(
	const HttpRouteConfig &from
) : _RequestPath(from._RequestPath),
		_Methods(from._Methods),
		_Redirect(from._Redirect),
		_DocumentRoot(from._DocumentRoot),
		_IsDocumentListingEnabled(from._IsDocumentListingEnabled),
		_IndexFileList(from._IndexFileList),
		_CgiConfigList(from._CgiConfigList),
		_RequestPathSegmentList(from._RequestPathSegmentList)
{
}

HttpRouteConfig &webserv::HttpRouteConfig::operator=(
	const HttpRouteConfig &from
)
{
	if (this == &from)
		return *this;

	this->_RequestPath = from._RequestPath;
	this->_Methods = from._Methods;
	this->_Redirect = from._Redirect;
	this->_DocumentRoot = from._DocumentRoot;
	this->_IsDocumentListingEnabled = from._IsDocumentListingEnabled;
	this->_IndexFileList = from._IndexFileList;
	this->_CgiConfigList = from._CgiConfigList;
	this->_RequestPathSegmentList = from._RequestPathSegmentList;

	return *this;
}

webserv::HttpRouteConfig::~HttpRouteConfig()
{
}

const std::string &HttpRouteConfig::getRequestPath() const
{
	return this->_RequestPath;
}
void HttpRouteConfig::setRequestPath(const std::string &RequestPath)
{
	this->_RequestPath = RequestPath;

	this->_RequestPathSegmentList = utils::split(this->_RequestPath, '/');
}

IMPL_REF_GETTER_SETTER_NS(std::vector<std::string>, Methods, HttpRouteConfig::)
IMPL_REF_GETTER_SETTER_NS(HttpRedirectConfig, Redirect, HttpRouteConfig::)
IMPL_REF_GETTER_SETTER_NS(std::string, DocumentRoot, HttpRouteConfig::)
IMPL_GETTER_SETTER_NS(bool, IsDocumentListingEnabled, HttpRouteConfig::)
IMPL_REF_GETTER_SETTER_NS(std::vector<std::string>, IndexFileList, HttpRouteConfig::)
IMPL_REF_GETTER_SETTER_NS(CgiConfigListType, CgiConfigList, HttpRouteConfig::)

const std::vector<std::string> &HttpRouteConfig::getRequestPathSegmentList() const
{
	return this->_RequestPathSegmentList;
}

}	 // namespace webserv
