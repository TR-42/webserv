#pragma once

#include <Logger.hpp>
#include <http/HttpRequest.hpp>
#include <set>
#include <string>
#include <types.hpp>
#include <utils/ErrorPageProvider.hpp>
#include <utils/UUID.hpp>

#include "./ServerConfig.hpp"

namespace webserv
{

class ServerRunningConfig
{
 private:
	uint16_t _port;
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
	bool isServerNameMatch(const HttpRequest &request) const;
	bool isSizeLimitExceeded(const HttpRequest &request) const;
	HttpRouteConfig pickRouteConfig(const HttpRequest &request) const;

	inline uint16_t getPort() const
	{
		return this->_port;
	}

	inline utils::ErrorPageProvider getErrorPageProvider() const
	{
		return this->_errorPageProvider;
	}
};

typedef std::vector<ServerRunningConfig> ServerRunningConfigListType;

}	 // namespace webserv
