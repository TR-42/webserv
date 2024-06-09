#include <config/validateConfig.hpp>
#include <utils/ErrorPageProvider.hpp>
#include <utils/to_string.hpp>

#define DECL_VALIDATOR(type) \
	static void validate##type( \
		const type &config, \
		std::vector<std::string> &errorMessageList, \
		const std::string &parentNodeInfo \
	)
#define DECL_VALIDATOR_ARG(type, ...) \
	static void validate##type( \
		const type &config, \
		std::vector<std::string> &errorMessageList, \
		const std::string &parentNodeInfo, \
		__VA_ARGS__ \
	)

namespace webserv
{

DECL_VALIDATOR(ListenConfig);
DECL_VALIDATOR_ARG(ServerConfigListType, uint16_t port);
DECL_VALIDATOR_ARG(ServerConfig, size_t index, std::set<std::string> &serverNameSet);
DECL_VALIDATOR_ARG(HttpRouteConfig, size_t index, std::set<std::string> &requestPathSet);
DECL_VALIDATOR(HttpRedirectConfig);
DECL_VALIDATOR(CgiConfigListType);
DECL_VALIDATOR_ARG(CgiConfig, std::set<std::string> &extSet);

static bool isUpper(const std::string &str)
{
	for (size_t i = 0; i < str.size(); ++i) {
		if (!std::isupper(str[i])) {
			return false;
		}
	}
	return true;
}

std::vector<std::string> validateConfig(
	const ListenConfig &listenConfig
)
{
	std::vector<std::string> errorMessageList;
	validateListenConfig(listenConfig, errorMessageList, "");
	return errorMessageList;
}

DECL_VALIDATOR(ListenConfig)
{
	if (config.getListenMap().empty()) {
		errorMessageList.push_back("No listen config found");
		return;
	}

	for (
		ListenMapType::const_iterator it = config.getListenMap().begin();
		it != config.getListenMap().end();
		++it
	) {
		validateServerConfigListType(it->second, errorMessageList, parentNodeInfo, it->first);
	}
}

DECL_VALIDATOR_ARG(ServerConfigListType, uint16_t port)
{
	(void)parentNodeInfo;
	std::string nodeInfo("ListenConfig[" + utils::to_string(port) + "]");
	if (config.empty()) {
		errorMessageList.push_back(nodeInfo + ": No server config found");
		return;
	}

	std::set<std::string> serverNameSet;
	for (size_t i = 0; i < config.size(); ++i) {
		validateServerConfig(config[i], errorMessageList, nodeInfo, i, serverNameSet);
	}
}

DECL_VALIDATOR_ARG(ServerConfig, size_t index, std::set<std::string> &serverNameSet)
{
	std::string nodeInfo(parentNodeInfo + ": ServerConfig[" + config.getKey() + "]");

	if (index != 0 && config.getServerNameList().empty()) {
		errorMessageList.push_back(nodeInfo + ": No server name found");
	}
	for (
		std::set<std::string>::const_iterator it = config.getServerNameList().begin();
		it != config.getServerNameList().end();
		++it
	) {
		// ポート単位で重複してないかチェック
		if (!serverNameSet.insert(*it).second) {
			errorMessageList.push_back(nodeInfo + ": Duplicate server name: " + *it);
		}
	}

	{
		utils::ErrorPageProvider errorPageProvider;
		for (
			ErrorPageMapType::const_iterator it = config.getErrorPageMap().begin();
			it != config.getErrorPageMap().end();
			++it
		) {
			if (!errorPageProvider.hasPage(it->first)) {
				errorMessageList.push_back(nodeInfo + ": Invalid error status code: " + utils::to_string(it->first));
			}
		}
	}

	if (config.getRouteList().empty()) {
		errorMessageList.push_back(nodeInfo + ": No route config found");
	}
	{
		std::set<std::string> requestPathSet;
		for (size_t i = 0; i < config.getRouteList().size(); ++i) {
			validateHttpRouteConfig(config.getRouteList()[i], errorMessageList, nodeInfo, i, requestPathSet);
		}
	}
}

DECL_VALIDATOR_ARG(HttpRouteConfig, size_t index, std::set<std::string> &requestPathSet)
{
	std::string nodeInfo(parentNodeInfo + ": HttpRouteConfig[" + utils::to_string(index) + "]");
	if (index == 0 && config.getRequestPath() != "/") {
		errorMessageList.push_back(nodeInfo + ": First route must be /");
	} else {
		if (!requestPathSet.insert(config.getRequestPath()).second) {
			errorMessageList.push_back(nodeInfo + ": Duplicate request path: " + config.getRequestPath());
		}
	}

	if (config.getDocumentRoot().empty() == config.getRedirect().getTo().empty()) {
		errorMessageList.push_back(nodeInfo + ": Either document root or redirect must be set");
	}

	if (config.getDocumentRoot().empty()) {
		validateHttpRedirectConfig(config.getRedirect(), errorMessageList, nodeInfo);
	}

	{
		std::set<std::string> methodSet;
		for (
			std::set<std::string>::const_iterator it = config.getMethods().begin();
			it != config.getMethods().end();
			++it
		) {
			// parse時点ではtoUpperしかしていないため、変なのが入ってないかチェック
			if (!isUpper(*it)) {
				errorMessageList.push_back(nodeInfo + ": Invalid method: " + *it);
			} else if (!methodSet.insert(*it).second) {
				errorMessageList.push_back(nodeInfo + ": Duplicate method: " + *it);
			}
		}
	}

	{
		std::set<std::string> indexFileSet;
		for (
			std::vector<std::string>::const_iterator it = config.getIndexFileList().begin();
			it != config.getIndexFileList().end();
			++it
		) {
			if (!indexFileSet.insert(*it).second) {
				errorMessageList.push_back(nodeInfo + ": Duplicate index file: " + *it);
			}
		}
	}

	if (!config.getCgiConfigList().empty()) {
		validateCgiConfigListType(config.getCgiConfigList(), errorMessageList, nodeInfo);
	}
}

DECL_VALIDATOR(HttpRedirectConfig)
{
	std::string nodeInfo(parentNodeInfo + ": HttpRedirectConfig");
	utils::ErrorPageProvider errorPageProvider;

	if (config.getTo().empty()) {
		errorMessageList.push_back(nodeInfo + ": No redirect target set");
	}

	if (!errorPageProvider.hasPage(config.getCode())) {
		errorMessageList.push_back(nodeInfo + ": Invalid status code: " + utils::to_string(config.getCode()));
	}
}

DECL_VALIDATOR(CgiConfigListType)
{
	if (config.empty()) {
		return;
	}

	std::set<std::string> extSet;
	for (
		CgiConfigListType::const_iterator it = config.begin();
		it != config.end();
		++it
	) {
		validateCgiConfig(*it, errorMessageList, parentNodeInfo, extSet);
	}
}

DECL_VALIDATOR_ARG(CgiConfig, std::set<std::string> &extSet)
{
	std::string nodeInfo(parentNodeInfo + ": CgiConfig[" + config.getKey() + "]");

	if (config.getExtensionWithoutDot().empty()) {
		errorMessageList.push_back(nodeInfo + ": No extension set");
	} else if (!extSet.insert(config.getExtensionWithoutDot()).second) {
		errorMessageList.push_back(nodeInfo + ": Duplicate extension: " + config.getExtensionWithoutDot());
	}
	if (config.getCgiExecutableFullPath().empty()) {
		errorMessageList.push_back(nodeInfo + ": No path set");
	}
}

}	 // namespace webserv
