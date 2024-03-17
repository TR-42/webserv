#pragma once

#include <map>
#include <string>

namespace webserv
{
namespace env
{

class EnvManager
{
 private:
	std::map<std::string, std::string> env;

 public:
	EnvManager();
	EnvManager(const char *envp[]);
	~EnvManager();

	size_t size() const;

	const std::string &get(const std::string &key) const;
	void set(const std::string &key, const std::string &value);

	std::string &operator[](const std::string &key);

	char **toEnvp() const;
};

}	 // namespace env
}	 // namespace webserv
