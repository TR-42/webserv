#include <EnvManager.hpp>
#include <config/CgiConfig.hpp>
#include <config/parseCgiConfig.hpp>
#include <stdexcept>
#include <utils/stoul.hpp>
#include <yaml/MappingNode.hpp>
#include <yaml/yamlUtils.hpp>

#define YAML_KEY_EXT "extensionWithoutDot"
#define YAML_KEY_CGI_FULLPATH "cgiExecutableFullPath"
#define YAML_KEY_ENV_PRESET "envPreset"

namespace webserv
{

static std::string toLower(const std::string &str)
{
	std::string lower(str);
	for (std::string::iterator it = lower.begin(); it != lower.end(); ++it) {
		if (std::isupper(*it)) {
			*it = *it - 'A' + 'a';
		}
	}
	return lower;
}

CgiConfig parseCgiConfig(const yaml::MappingNode &node)
{
	if (!node.has(YAML_KEY_CGI_FULLPATH) || !node.has(YAML_KEY_EXT))
		throw std::runtime_error("CgiConfig[" + node.getKey() + "]: " YAML_KEY_CGI_FULLPATH " and " YAML_KEY_EXT " are required");

	std::string yaml_ext = toLower(yaml::getScalarNode(node, YAML_KEY_EXT).getValue());
	std::string yaml_cgi_fullpath = yaml::getScalarNode(node, YAML_KEY_CGI_FULLPATH).getValue();

	if (yaml_cgi_fullpath.empty())
		throw std::runtime_error("CgiConfig[" + node.getKey() + "]: " YAML_KEY_CGI_FULLPATH " must be a non-empty string");
	if (yaml_ext.empty())
		throw std::runtime_error("CgiConfig[" + node.getKey() + "]: " YAML_KEY_ENV_PRESET " must be a non-empty string");

	env::EnvManager env;

	if (!node.has(YAML_KEY_ENV_PRESET))
		return CgiConfig(node.getKey(), yaml_ext, yaml_cgi_fullpath, env);

	const yaml::MappingNode &env_node = yaml::getMappingNode(node, YAML_KEY_ENV_PRESET);

	for (yaml::NodeVector::const_iterator it = env_node.getNodes().begin(); it != env_node.getNodes().end(); ++it) {
		const yaml::ScalarNode &env_entry = yaml::getScalarNode(**it);
		env.set(env_entry.getKey(), env_entry.getValue());
	}

	return CgiConfig(node.getKey(), yaml_ext, yaml_cgi_fullpath, env);
}

}	 // namespace webserv
