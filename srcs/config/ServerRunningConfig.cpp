#include <Logger.hpp>
#include <config/ServerConfig.hpp>
#include <config/ServerRunningConfig.hpp>
#include <http/HttpRequest.hpp>
#include <iostream>
#include <set>
#include <stdexcept>
#include <utils/ErrorPageProvider.hpp>

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
		logger(logger)
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

HttpRouteConfig webserv::ServerRunningConfig::pickRouteConfig(
	const HttpRequest &request
) const
{
	// TODO: パスに応じたRouteConfigの選択
	(void)request;
	return this->_routeList[0];
}

ServerRunningConfig::~ServerRunningConfig()
{
}

}	 // namespace webserv
