#include <limits.h>
#include <stdlib.h>

#include <config/parseHttpRouteConfig.hpp>
#include <config/parseServerConfig.hpp>
#include <stdexcept>
#include <utils/stoul.hpp>
#include <yaml/MappingNode.hpp>
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
		throw std::runtime_error("HttpRouteConfig: " YAML_KEY_SERVER_NAME_LIST " is required");
	if (!node.has(YAML_KEY_PORT))
		throw std::runtime_error("HttpRouteConfig: " YAML_KEY_PORT " is required");
	if (!node.has(YAML_KEY_ROUTE_LIST))
		throw std::runtime_error("HttpRouteConfig: " YAML_KEY_ROUTE_LIST " is required");

	std::vector<std::string> serverNameList;
	uint16_t port;
	std::size_t requestBodyLimit = 0;
	ErrorPageMapType errorPageMap;
	RouteListType routeList;

	const yaml::MappingNode &serverNameListNode = yaml::getMappingNode(node, YAML_KEY_SERVER_NAME_LIST);
	if (serverNameListNode.getNodes().empty())
		throw std::runtime_error("ServerConfig: " YAML_KEY_SERVER_NAME_LIST " must not be empty");
	for (yaml::NodeVector::const_iterator it = serverNameListNode.getNodes().begin(); it != serverNameListNode.getNodes().end(); ++it) {
		serverNameList.push_back(yaml::getScalarNode(**it).getKey());
	}

	unsigned long port_ulong;
	if (!utils::stoul(yaml::getScalarNode(node, YAML_KEY_PORT).getValue(), port_ulong))
		throw std::runtime_error("ServerConfig: " YAML_KEY_PORT " must be a positive integer");

	if (0 < port_ulong && port_ulong <= UINT16_MAX)
		port = static_cast<uint16_t>(port_ulong);
	else
		throw std::runtime_error("ServerConfig: " YAML_KEY_PORT " must be a positive integer");

	// TODO: requestBodyLimit, errorPageMap, routeList実装する

	return ServerConfig(serverNameList, port, requestBodyLimit, errorPageMap, routeList);
}

}	 // namespace webserv
