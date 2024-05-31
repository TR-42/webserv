#pragma once
#include <string>
#include <yaml/MappingNode.hpp>

namespace webserv
{

class HttpRedirectConfig
{
 private:
	std::string _redirectFrom;
	std::string _redirectTo;

 public:
	HttpRedirectConfig(const std::string &redirectFrom, const std::string &redirectTo) : _redirectFrom(redirectFrom), _redirectTo(redirectTo) {}
	const std::string &getRedirectFrom() const { return this->_redirectFrom; }
	const std::string &getRedirectTo() const { return this->_redirectTo; }
	HttpRedirectConfig parseHttpRedirectConfig(const yaml::MappingNode &node);
	HttpRedirectConfig config(std::string redirectFrom, std::string redirectTo);
};

}	 // namespace webserv
