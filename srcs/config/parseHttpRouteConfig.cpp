#include <limits.h>
#include <stdlib.h>

#include <config/HttpRouteConfig.hpp>
#include <config/parseCgiConfig.hpp>
#include <config/parseHttpRedirectConfig.hpp>
#include <config/parseHttpRouteConfig.hpp>
#include <stdexcept>
#include <utils/stoul.hpp>
#include <yaml/MappingNode.hpp>
#include <yaml/yamlUtils.hpp>

#define YAML_KEY_REQUEST_PATH "request_path"
#define YAML_KEY_METHODS "methods"
#define YAML_KEY_REDIRECT "redirect"
#define YAML_KEY_DOCUMENT_ROOT "document_root"
#define YAML_KEY_DOCUMENT_LISTING "document_listing"
#define YAML_KEY_INDEX_FILES "index_files"
#define YAML_KEY_CGI "cgi"

namespace webserv
{

HttpRouteConfig parseHttpRouteConfig(const yaml::MappingNode &node, const std::string &yamlFilePath)
{
	if (!node.has(YAML_KEY_REQUEST_PATH))
		throw std::runtime_error("HttpRouteConfig[" + node.getKey() + "]: " YAML_KEY_REQUEST_PATH " is required");
	if (node.has(YAML_KEY_DOCUMENT_ROOT) == node.has(YAML_KEY_REDIRECT))
		throw std::runtime_error("HttpRouteConfig[" + node.getKey() + "]: " YAML_KEY_DOCUMENT_ROOT " and " YAML_KEY_REDIRECT " must not be both present or both absent");

	std::string yaml_request_path = yaml::getScalarNode(node, YAML_KEY_REQUEST_PATH).getValue();
	std::vector<std::string> yaml_methods;
	HttpRedirectConfig yaml_redirect;
	std::string yaml_document_root;
	bool yaml_document_listing = false;
	std::vector<std::string> yaml_index_files;
	std::vector<CgiConfig> yaml_cgi;

	if (yaml_request_path.empty() || yaml_request_path[0] != '/')
		throw std::runtime_error("HttpRouteConfig[" + node.getKey() + "]: " YAML_KEY_REQUEST_PATH " must start with a '/'");

	if (node.has(YAML_KEY_METHODS)) {
		const yaml::MappingNode &methods_node = yaml::getMappingNode(node, YAML_KEY_METHODS);
		for (yaml::NodeVector::const_iterator it = methods_node.getNodes().begin(); it != methods_node.getNodes().end(); ++it) {
			yaml_methods.push_back(yaml::getMappingNode(**it).getKey());
		}
	}

	if (node.has(YAML_KEY_REDIRECT)) {
		yaml_redirect = parseHttpRedirectConfig(yaml::getMappingNode(node, YAML_KEY_REDIRECT));
	}

	if (node.has(YAML_KEY_DOCUMENT_ROOT)) {
		yaml_document_root = yaml::getScalarNode(node, YAML_KEY_DOCUMENT_ROOT).getValue();
		if (yaml_document_root.empty())
			throw std::runtime_error("HttpRouteConfig[" + node.getKey() + "]: " YAML_KEY_DOCUMENT_ROOT " must not be empty");
	}

	if (yaml_document_root[0] != '/') {
		yaml_document_root = yamlFilePath.substr(0, yamlFilePath.find_last_of('/') + 1) + yaml_document_root;
		char resolved_path[PATH_MAX];
		if (realpath(yaml_document_root.c_str(), resolved_path) == NULL) {
			throw std::runtime_error("HttpRouteConfig[" + node.getKey() + "]: " YAML_KEY_DOCUMENT_ROOT " is not a valid path");
		}
		yaml_document_root = resolved_path;
	}

	if (node.has(YAML_KEY_DOCUMENT_LISTING)) {
		yaml_document_listing = yaml::getScalarNode(node, YAML_KEY_DOCUMENT_LISTING).getValue() == "true";
	}

	if (node.has(YAML_KEY_INDEX_FILES)) {
		const yaml::MappingNode &index_files_node = yaml::getMappingNode(node, YAML_KEY_INDEX_FILES);
		for (yaml::NodeVector::const_iterator it = index_files_node.getNodes().begin(); it != index_files_node.getNodes().end(); ++it) {
			yaml_index_files.push_back(yaml::getMappingNode(**it).getKey());
		}
	}

	if (node.has(YAML_KEY_CGI)) {
		const yaml::MappingNode &cgi_node = yaml::getMappingNode(node, YAML_KEY_CGI);
		for (yaml::NodeVector::const_iterator it = cgi_node.getNodes().begin(); it != cgi_node.getNodes().end(); ++it) {
			yaml_cgi.push_back(parseCgiConfig(yaml::getMappingNode(**it)));
		}
	}

	return HttpRouteConfig(
		yaml_request_path,
		yaml_methods,
		yaml_redirect,
		yaml_document_root,
		yaml_document_listing,
		yaml_index_files,
		yaml_cgi
	);
}

}	 // namespace webserv
