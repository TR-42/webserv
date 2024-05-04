#pragma once

#include <Logger.hpp>
#include <http/HttpRequest.hpp>
#include <set>
#include <string>
#include <utils/ErrorPageProvider.hpp>

#include "./ServerConfig.hpp"

namespace webserv
{

class ServerRunningConfig
{
 private:
	std::set<std::string> _serverNameList;
	utils::ErrorPageProvider _errorPageProvider;
	size_t _requestBodyLimit;
	RouteListType _routeList;
	const Logger &logger;

 public:
	ServerRunningConfig(
		const ServerConfig &serverConfig,
		utils::ErrorPageProvider &errorPageProvider,
		Logger &logger
	);
	~ServerRunningConfig();

	bool isServerNameMatch(const std::string &serverName) const;
	bool isServerNameMatch(const HttpRequest &request) const;
	bool isSizeLimitExceeded(const HttpRequest &request) const;
	HttpRouteConfig pickRouteConfig(const HttpRequest &request) const;
};

typedef std::vector<ServerRunningConfig> ServerRunningConfigListType;

}	 // namespace webserv
