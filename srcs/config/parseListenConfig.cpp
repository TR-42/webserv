#include <config/parseListenConfig.hpp>
#include <config/parseServerConfig.hpp>
#include <yaml/yamlUtils.hpp>

namespace webserv
{

ListenConfig parseListenConfig(const yaml::MappingNode &node, const std::string &yamlFilePath)
{
	ListenMapType listenMap;
	for (yaml::NodeVector::const_iterator it = node.getNodes().begin(); it != node.getNodes().end(); ++it) {
		const yaml::MappingNode &yaml_server_node = yaml::getMappingNode(**it);
		ServerConfig serverConfig = parseServerConfig(yaml_server_node, yamlFilePath);
		uint16_t port = serverConfig.getPort();
		if (listenMap.find(port) == listenMap.end())
			listenMap[port] = std::vector<ServerConfig>();
		listenMap[port].push_back(serverConfig);
	}
	return ListenConfig(listenMap);
}

}	 // namespace webserv
