#include <EnvManager.hpp>
#include <cstring>

extern char **environ;

static void _set_env_map(
	std::map<std::string, std::string> &env,
	const char *envp[]
)
{
	if (envp == NULL) {
		return;
	}

	while (*envp != NULL) {
		const std::string envKeyValuePair(*envp);
		size_t pos = envKeyValuePair.find('=');
		if (pos != std::string::npos) {
			env[envKeyValuePair.substr(0, pos)] = envKeyValuePair.substr(pos + 1);
		}

		envp++;
	}
}

webserv::env::EnvManager::EnvManager()
{
	this->env.clear();
	_set_env_map(this->env, (const char **)environ);
}

webserv::env::EnvManager::EnvManager(const char *envp[])
{
	this->env.clear();
	_set_env_map(this->env, envp);
}

webserv::env::EnvManager::~EnvManager()
{
}

size_t webserv::env::EnvManager::size() const
{
	return this->env.size();
}

const std::string &webserv::env::EnvManager::get(const std::string &key) const
{
	return this->env.at(key);
}

void webserv::env::EnvManager::set(const std::string &key, const std::string &value)
{
	this->env[key] = value;
}

std::string &webserv::env::EnvManager::operator[](const std::string &key)
{
	return this->env[key];
}

char **webserv::env::EnvManager::toEnvp() const
{
	const size_t envpSize = this->env.size();
	char **envp = new char *[envpSize + 1];
	std::memset(envp, 0, sizeof(char *) * (envpSize + 1));

	size_t i = 0;
	try {
		for (std::map<std::string, std::string>::const_iterator it = this->env.cbegin(); it != this->env.cend(); ++it) {
			const std::string envKeyValuePair = it->first + "=" + it->second;
			const size_t envKeyValuePairSize = envKeyValuePair.length();
			envp[i] = new char[envKeyValuePairSize + 1];
			std::memcpy(envp[i], envKeyValuePair.c_str(), envKeyValuePairSize + 1);
			i++;
		}
		return envp;
	} catch (...) {
		i = 0;
		while (envp[i] != NULL) {
			delete[] envp[i];
			i++;
		}
		delete[] envp;
		throw;
	}
}
