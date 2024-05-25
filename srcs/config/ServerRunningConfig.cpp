#include <Logger.hpp>
#include <config/ServerConfig.hpp>
#include <config/ServerRunningConfig.hpp>
#include <iostream>
#include <set>
#include <stdexcept>
#include <utils/ErrorPageProvider.hpp>
#include <utils/UUIDv7.hpp>

namespace webserv
{

static void _loadErrorPageContent(
	const ErrorPageMapType &errorPageMap,
	utils::ErrorPageProvider &targetErrorPageProvider,
	const Logger &logger
)
{
	const ErrorPageMapType::const_iterator itEnd = errorPageMap.end();
	for (
		ErrorPageMapType::const_iterator it = errorPageMap.begin();
		it != itEnd;
		++it
	) {
		uint16_t errorCode = it->first;
		std::string errorPagePath = it->second;
		if (errorPagePath.empty()) {
			LS_ERROR()
				<< "ErrorPage path for error code " << (int)errorCode << " was empty"
				<< std::endl;
			throw std::runtime_error("ErrorPage path was empty");
		}

		targetErrorPageProvider.setErrorPageFromFile(errorCode, errorPagePath);
	}
}

static inline std::string _ConvertToLowerCase(std::string str)
{
	size_t strLength = str.size();
	for (size_t i = 0; i < strLength; ++i) {
		str[i] = std::tolower(str[i]);
	}
	return str;
}

static inline std::set<std::string> _ServerNameVecToSet(const std::vector<std::string> &strList)
{
	std::set<std::string> serverNameSet;

	std::vector<std::string>::const_iterator itEnd = strList.end();
	for (
		std::vector<std::string>::const_iterator it = strList.begin();
		it != itEnd;
		++it
	) {
		serverNameSet.insert(_ConvertToLowerCase(*it));
	}

	return serverNameSet;
}

ServerRunningConfig::ServerRunningConfig(
	const ServerConfig &serverConfig,
	utils::ErrorPageProvider &errorPageProvider,
	Logger &logger
) : _serverNameList(_ServerNameVecToSet(serverConfig.getServerNameList())),
		_errorPageProvider(errorPageProvider),
		_requestBodyLimit(serverConfig.getRequestBodyLimit()),
		_routeList(serverConfig.getRouteList()),
		_uuid(utils::UUIDv7()),
		logger(Logger(logger, "ServerConfig=" + this->_uuid.toString()))
{
	if (this->_routeList.empty()) {
		LS_FATAL() << "RouteList is empty" << std::endl;
		throw std::runtime_error("RouteList is empty");
	}

	_loadErrorPageContent(
		serverConfig.getErrorPageMap(),
		this->_errorPageProvider,
		this->logger
	);

	LS_DEBUG()
		<< "ServerRunningConfig created"
		<< " serverNameList.size()=" << this->_serverNameList.size()
		<< " requestBodyLimit=" << this->_requestBodyLimit
		<< " routeList.size()=" << this->_routeList.size()
		<< std::endl;
}

ServerRunningConfig::ServerRunningConfig(
	const ServerRunningConfig &from
) : _serverNameList(from._serverNameList),
		_errorPageProvider(from._errorPageProvider),
		_requestBodyLimit(from._requestBodyLimit),
		_routeList(from._routeList),
		_uuid(from._uuid),
		logger(from.logger)
{
}

ServerRunningConfig &webserv::ServerRunningConfig::operator=(const ServerRunningConfig &from)
{
	(void)from;
	throw std::runtime_error("ServerRunningConfig copy assignment operator is not allowed");
}

bool webserv::ServerRunningConfig::isServerNameMatch(
	const std::string &serverName
) const
{
	std::string serverNameLower = serverName;
	const size_t serverNameLength = serverNameLower.size();
	for (size_t i = 0; i < serverNameLength; ++i) {
		if (serverNameLower[i] == ':') {
			serverNameLower = _ConvertToLowerCase(serverNameLower.substr(0, i));
			break;
		}
	}

	return this->_serverNameList.find(serverNameLower) != this->_serverNameList.end();
}

bool webserv::ServerRunningConfig::isSizeLimitExceeded(
	const size_t contentLength
) const
{
	if (this->_requestBodyLimit < contentLength) {
		return true;
	}

	return false;
}

static size_t getMatchedLength(
	const std::string &requestedPath,
	const std::string &pathRule
)
{
	size_t requestedPathLength = requestedPath.size();
	size_t pathRuleLength = pathRule.size();
	if (requestedPathLength < pathRuleLength) {
		return 0;
	}

	for (size_t i = 0; i < pathRuleLength; ++i) {
		if (requestedPath[i] != pathRule[i]) {
			return 0;
		}
	}

	return pathRuleLength;
}

HttpRouteConfig webserv::ServerRunningConfig::pickRouteConfig(
	const std::string &path
) const
{
	const HttpRouteConfig *matchedRouteConfig = &(this->_routeList[0]);
	size_t routeListSize = this->_routeList.size();
	if (routeListSize == 1) {
		return *matchedRouteConfig;
	}

	size_t matchedPathRuleLength = 0;
	CS_DEBUG()
		<< "Picking route config for path: "
		<< "path=" << path
		<< std::endl;
	for (
		size_t i = 0;
		i < routeListSize;
		++i
	) {
		const HttpRouteConfig &routeConfig = this->_routeList[i];
		size_t pathRuleLength = getMatchedLength(path, routeConfig.getRequestPath());
		if (pathRuleLength != 0) {
			CS_DEBUG()
				<< "Matched path rule: "
				<< "path=" << path
				<< " pathRule=" << routeConfig.getRequestPath()
				<< std::endl;
		}
		if (matchedPathRuleLength < pathRuleLength) {
			matchedPathRuleLength = pathRuleLength;
			matchedRouteConfig = &(this->_routeList[i]);
		}
	}

	CS_DEBUG()
		<< "Picked route config: "
		<< "path=" << path
		<< " pathRule=" << matchedRouteConfig->getRequestPath()
		<< " documentRoot=" << matchedRouteConfig->getDocumentRoot()
		<< std::endl;

	return *matchedRouteConfig;
}

ServerRunningConfig::~ServerRunningConfig()
{
}

}	 // namespace webserv
