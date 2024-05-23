#include <service/getRequestedFilePath.hpp>
#include <utils/normalizePath.hpp>

namespace webserv
{

std::string getRequestedFilePath(
	const HttpRouteConfig &routeConfig,
	const std::string &requestPath
)
{
	std::string documentRoot = routeConfig.getDocumentRoot();
	if (documentRoot.empty()) {
		return utils::normalizePath("./" + requestPath);
	}

	size_t ruleLength = routeConfig.getRequestPath().size();
	if (requestPath.size() <= ruleLength) {
		return documentRoot;
	}

	std::string path = requestPath.substr(ruleLength);
	return utils::normalizePath(documentRoot + "/" + path);
}

}	 // namespace webserv
