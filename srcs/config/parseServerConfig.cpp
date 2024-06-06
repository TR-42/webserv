#include <limits.h>
#include <stdlib.h>

#include <config/parseHttpRouteConfig.hpp>
#include <config/parseServerConfig.hpp>
#include <stdexcept>
#include <utils/stoul.hpp>
#include <yaml/MappingNode.hpp>
#include <yaml/ScalarNode.hpp>
#include <yaml/yamlUtils.hpp>

#define YAML_KEY_SERVER_NAME_LIST "serverNameList"
#define YAML_KEY_PORT "port"
#define YAML_KEY_REQUEST_BODY_LIMIT "requestBodyLimit"
#define YAML_KEY_ERROR_PAGES "errorPages"
#define YAML_KEY_ROUTE_LIST "routeList"

namespace webserv
{

ServerConfig parseServerConfig(const yaml::MappingNode &node, const std::string &yamlFilePath)
{
	if (!node.has(YAML_KEY_SERVER_NAME_LIST))
		throw std::runtime_error("HttpRouteConfig[" + node.getKey() + "]: " YAML_KEY_SERVER_NAME_LIST " is required");
	if (!node.has(YAML_KEY_PORT))
		throw std::runtime_error("HttpRouteConfig[" + node.getKey() + "]: " YAML_KEY_PORT " is required");
	if (!node.has(YAML_KEY_ROUTE_LIST))
		throw std::runtime_error("HttpRouteConfig[" + node.getKey() + "]: " YAML_KEY_ROUTE_LIST " is required");

	std::vector<std::string> serverNameList;
	uint16_t port;
	std::size_t requestBodyLimit = 0;
	ErrorPageMapType errorPageMap;
	RouteListType routeList;

	const yaml::MappingNode &serverNameListNode = yaml::getMappingNode(node, YAML_KEY_SERVER_NAME_LIST);
	if (serverNameListNode.getNodes().empty())
		throw std::runtime_error("ServerConfig[" + node.getKey() + "]: " YAML_KEY_SERVER_NAME_LIST " must not be empty");
	for (yaml::NodeVector::const_iterator it = serverNameListNode.getNodes().begin(); it != serverNameListNode.getNodes().end(); ++it) {
		serverNameList.push_back(yaml::getMappingNode(**it).getKey());
	}

	unsigned long port_ulong;
	if (!utils::stoul(yaml::getScalarNode(node, YAML_KEY_PORT).getValue(), port_ulong))
		throw std::runtime_error("ServerConfig[" + node.getKey() + "]: " YAML_KEY_PORT " must be a positive integer");

	if (0 < port_ulong && port_ulong <= UINT16_MAX)
		port = static_cast<uint16_t>(port_ulong);
	else
		throw std::runtime_error("ServerConfig[" + node.getKey() + "]: " YAML_KEY_PORT " must be a positive integer");

	if (node.has(YAML_KEY_REQUEST_BODY_LIMIT)) {
		unsigned long requestBodyLimit_ulong;
		if (!utils::stoul(yaml::getScalarNode(node, YAML_KEY_REQUEST_BODY_LIMIT).getValue(), requestBodyLimit_ulong))
			throw std::runtime_error("ServerConfig[" + node.getKey() + "]: " YAML_KEY_REQUEST_BODY_LIMIT " must be a positive integer");
		requestBodyLimit = static_cast<std::size_t>(requestBodyLimit_ulong);
	}

	if (node.has(YAML_KEY_ERROR_PAGES)) {
		const yaml::MappingNode &errorPagesNode = yaml::getMappingNode(node, YAML_KEY_ERROR_PAGES);
		for (yaml::NodeVector::const_iterator it = errorPagesNode.getNodes().begin(); it != errorPagesNode.getNodes().end(); ++it) {
			const yaml::ScalarNode &errorPageNode = yaml::getScalarNode(**it);
			if (errorPageNode.getValue().empty())
				throw std::runtime_error("ServerConfig[" + node.getKey() + "]: " YAML_KEY_ERROR_PAGES " must not contain empty value nodes");
			std::string errorPageFilePath = errorPageNode.getValue();
			if (errorPageFilePath[0] != '/') {
				errorPageFilePath = yamlFilePath.substr(0, yamlFilePath.find_last_of('/') + 1) + errorPageFilePath;
				char resolved_path[PATH_MAX];
				if (realpath(errorPageFilePath.c_str(), resolved_path) == NULL) {
					throw std::runtime_error("ServerConfig[" + node.getKey() + "]: " YAML_KEY_ERROR_PAGES " contains an invalid path");
				}
				errorPageFilePath = resolved_path;
			}
			unsigned long errorPageCode;
			if (!utils::stoul(errorPageNode.getKey(), errorPageCode))
				throw std::runtime_error("ServerConfig[" + node.getKey() + "]: " YAML_KEY_ERROR_PAGES " must contain positive integer keys");
			if (errorPageCode <= 0 || 1000 <= errorPageCode)
				throw std::runtime_error("ServerConfig[" + node.getKey() + "]: " YAML_KEY_ERROR_PAGES " must contain keys between 1 and 999");
			errorPageMap[static_cast<uint16_t>(errorPageCode)] = errorPageFilePath;
		}
	}

	const yaml::MappingNode &routeListNode = yaml::getMappingNode(node, YAML_KEY_ROUTE_LIST);
	if (routeListNode.getNodes().empty())
		throw std::runtime_error("ServerConfig[" + node.getKey() + "]: " YAML_KEY_ROUTE_LIST " must not be empty");
	for (yaml::NodeVector::const_iterator it = routeListNode.getNodes().begin(); it != routeListNode.getNodes().end(); ++it) {
		routeList.push_back(parseHttpRouteConfig(yaml::getMappingNode(**it), yamlFilePath));
	}

	return ServerConfig(serverNameList, port, requestBodyLimit, errorPageMap, routeList);
}

}	 // namespace webserv