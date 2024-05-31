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

HttpRouteConfig &HttpRouteConfig::operator=(
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

HttpRouteConfig::~HttpRouteConfig()
{
}

void HttpRouteConfig::setRequestPath(const std::string &RequestPath)
{
	this->_RequestPath = RequestPath;

	this->_RequestPathSegmentList = utils::split(this->_RequestPath, '/');
}

}	 // namespace webserv
