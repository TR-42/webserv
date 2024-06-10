#pragma once

#include <Logger.hpp>
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
	size_t _timeoutMs;
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
	HttpRouteConfig pickRouteConfig(
		const std::vector<std::string> &pathSegmentList,
		const std::string &method
	) const;

	inline uint16_t getPort() const
	{
		return this->_port;
	}

	inline size_t getTimeoutMs() const
	{
		return this->_timeoutMs;
	}

	inline utils::ErrorPageProvider getErrorPageProvider() const
	{
		return this->_errorPageProvider;
	}

	inline size_t getRequestBodyLimit() const
	{
		return this->_requestBodyLimit;
	}
};

typedef std::vector<ServerRunningConfig> ServerRunningConfigListType;

}	 // namespace webserv
