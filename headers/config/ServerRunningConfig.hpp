#pragma once

#include <Logger.hpp>
#include <set>
#include <string>
#include <utils/ErrorPageProvider.hpp>
#include <utils/UUID.hpp>

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
	utils::UUID _uuid;
	Logger logger;

	ServerRunningConfig &operator=(const ServerRunningConfig &from);

 public:
	ServerRunningConfig(
		const ServerConfig &serverConfig,
		utils::ErrorPageProvider &errorPageProvider,
		Logger &logger
	);
	ServerRunningConfig(const ServerRunningConfig &from);
	virtual ~ServerRunningConfig();

	bool isServerNameMatch(const std::string &serverName) const;
	bool isSizeLimitExceeded(const size_t contentLength) const;
	HttpRouteConfig pickRouteConfig(const std::string &path) const;

	inline utils::ErrorPageProvider getErrorPageProvider() const
	{
		return this->_errorPageProvider;
	}
};

typedef std::vector<ServerRunningConfig> ServerRunningConfigListType;

}	 // namespace webserv
