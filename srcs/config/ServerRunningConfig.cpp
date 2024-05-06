#include <Logger.hpp>
#include <config/ServerConfig.hpp>
#include <config/ServerRunningConfig.hpp>
#include <http/HttpRequest.hpp>
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
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
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

bool webserv::ServerRunningConfig::isServerNameMatch(
	const std::string &serverName
) const
{
	// TODO: ポート番号部分を取り除く
	std::string serverNameLower = serverName;
	std::transform(serverNameLower.begin(), serverNameLower.end(), serverNameLower.begin(), ::tolower);

	return this->_serverNameList.find(serverNameLower) != this->_serverNameList.end();
}

bool webserv::ServerRunningConfig::isServerNameMatch(
	const HttpRequest &request
) const
{
	return this->isServerNameMatch(request.getHost());
}

bool webserv::ServerRunningConfig::isSizeLimitExceeded(
	const HttpRequest &request
) const
{
	if (this->_requestBodyLimit < request.getBody().size()) {
		return true;
	}

	return false;
}

static size_t getMatchedLength(
	const std::string &requestedPath,
	const std::string &pathRUle
)
{
	size_t requestedPathLength = requestedPath.size();
	size_t pathRuleLength = pathRUle.size();
	if (requestedPathLength < pathRuleLength) {
		return 0;
	}

	for (size_t i = 0; i < pathRuleLength; ++i) {
		if (requestedPath[i] != pathRUle[i]) {
			return 0;
		}
	}
	return pathRuleLength;
}

HttpRouteConfig webserv::ServerRunningConfig::pickRouteConfig(
	const HttpRequest &request
) const
{
	const HttpRouteConfig *matchedRouteConfig = &(this->_routeList[0]);
	size_t routeListSize = this->_routeList.size();
	if (routeListSize == 1) {
		return *matchedRouteConfig;
	}

	size_t matchedPathRuleLength = 0;
	std::string path = request.getPath();
	for (
		size_t i = 0;
		i < routeListSize;
		++i
	) {
		const HttpRouteConfig &routeConfig = this->_routeList[i];
		size_t pathRuleLength = getMatchedLength(path, routeConfig.getRequestPath());
		if (matchedPathRuleLength < pathRuleLength) {
			matchedPathRuleLength = pathRuleLength;
			matchedRouteConfig = &(this->_routeList[i]);
		}
	}

	return *matchedRouteConfig;
}

ServerRunningConfig::~ServerRunningConfig()
{
}

}	 // namespace webserv
