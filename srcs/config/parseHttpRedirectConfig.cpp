#include <config/HttpRedirectConfig.hpp>
#include <config/parseHttpRedirectConfig.hpp>
#include <stdexcept>
#include <utils/stoul.hpp>
#include <yaml/MappingNode.hpp>
#include <yaml/yamlUtils.hpp>

#define YAML_KEY_TO "to"
#define YAML_KEY_CODE "code"

namespace webserv
{

HttpRedirectConfig parseHttpRedirectConfig(const yaml::MappingNode &node)
{
	if (!node.has(YAML_KEY_TO) || !node.has(YAML_KEY_CODE))
		throw std::runtime_error("HttpRedirectConfig[" + node.getKey() + "]: " YAML_KEY_TO " and " YAML_KEY_CODE " are required");

	std::string yaml_to = yaml::getScalarNode(node, YAML_KEY_TO).getValue();
	std::string yaml_code = yaml::getScalarNode(node, YAML_KEY_CODE).getValue();

	if (yaml_to.empty())
		throw std::runtime_error("HttpRedirectConfig[" + node.getKey() + "]: " YAML_KEY_TO " must be a non-empty string");
	if (yaml_code.empty())
		throw std::runtime_error("HttpRedirectConfig[" + node.getKey() + "]: " YAML_KEY_CODE " must be a non-empty string");

	unsigned long code_ul = 0;
	if (!utils::stoul(yaml_code, code_ul))
		throw std::runtime_error("HttpRedirectConfig[" + node.getKey() + "]: " YAML_KEY_CODE " must be a number");
	if (code_ul <= 0 || 999 < code_ul)
		throw std::runtime_error("HttpRedirectConfig[" + node.getKey() + "]: " YAML_KEY_CODE " must be a number between 1 and 999");

	return HttpRedirectConfig(yaml_to, static_cast<uint16_t>(code_ul));
}

}	 // namespace webserv
