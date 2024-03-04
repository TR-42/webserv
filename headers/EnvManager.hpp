#pragma once

#include <map>
#include <string>

class EnvManager
{
 private:
	std::map<std::string, std::string> env;

 public:
	EnvManager(const char *envp[]);
	~EnvManager();

	const std::string &get(const std::string &key) const;
	void set(const std::string &key, const std::string &value);

	std::string &operator[](const std::string &key);

	char **toEnvp() const;
};
