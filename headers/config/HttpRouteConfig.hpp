#pragma once

#include <map>
#include <string>
#include <vector>

#include "../classDefUtils.hpp"
#include "./CgiConfig.hpp"
#include "./HttpRedirectConfig.hpp"

#define EQ(name) (lhs._##name == rhs._##name)

namespace webserv
{

typedef std::vector<CgiConfig> CgiConfigListType;

class HttpRouteConfig
{
	DECL_VAR_REF_GETTER(std::string, RequestPath)
	DECL_VAR_REF_GETTER_SETTER(std::vector<std::string>, Methods)
	DECL_VAR_REF_GETTER_SETTER(HttpRedirectConfig, Redirect)
	DECL_VAR_REF_GETTER_SETTER(std::string, DocumentRoot)
	DECL_VAR_GETTER_SETTER(bool, IsDocumentListingEnabled)
	DECL_VAR_REF_GETTER_SETTER(std::vector<std::string>, IndexFileList)
	DECL_VAR_REF_GETTER_SETTER(CgiConfigListType, CgiConfigList)
	DECL_VAR_REF_GETTER(std::vector<std::string>, RequestPathSegmentList)

 public:
	HttpRouteConfig();
	HttpRouteConfig(
		const std::string &RequestPath,
		const std::vector<std::string> &Methods,
		const HttpRedirectConfig &Redirect,
		const std::string &DocumentRoot,
		bool IsDocumentListingEnabled,
		const std::vector<std::string> &IndexFileList,
		const CgiConfigListType &CgiConfigList
	);
	HttpRouteConfig(const HttpRouteConfig &from);
	virtual ~HttpRouteConfig();
	HttpRouteConfig &operator=(const HttpRouteConfig &from);

	void setRequestPath(const std::string &RequestPath);

	friend bool operator==(const HttpRouteConfig &lhs, const HttpRouteConfig &rhs)
	{
		return (EQ(RequestPath) && EQ(Methods) && EQ(Redirect) && EQ(DocumentRoot) && EQ(IsDocumentListingEnabled) && EQ(IndexFileList) && EQ(CgiConfigList));
	}

	friend bool operator!=(const HttpRouteConfig &lhs, const HttpRouteConfig &rhs)
	{
		return !(lhs == rhs);
	}
};

}	 // namespace webserv
