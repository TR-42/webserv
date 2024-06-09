#include <config/CgiConfig.hpp>

namespace webserv
{

CgiConfig::CgiConfig()
{
}

CgiConfig::CgiConfig(
	const std::string &key,
	const std::string &extensionWithoutDot,
	const std::string &cgiExecutableFullPath,
	const env::EnvManager &envPreset
) : _Key(key),
		_ExtensionWithoutDot(extensionWithoutDot),
		_CgiExecutableFullPath(cgiExecutableFullPath),
		_EnvPreset(envPreset)
{
}

CgiConfig::CgiConfig(
	const CgiConfig &from
) : _Key(from._Key),
		_ExtensionWithoutDot(from._ExtensionWithoutDot),
		_CgiExecutableFullPath(from._CgiExecutableFullPath),
		_EnvPreset(from._EnvPreset)
{
}

CgiConfig &CgiConfig::operator=(
	const CgiConfig &from
)
{
	if (this == &from)
		return *this;

	this->_Key = from._Key;
	this->_ExtensionWithoutDot = from._ExtensionWithoutDot;
	this->_CgiExecutableFullPath = from._CgiExecutableFullPath;
	this->_EnvPreset = from._EnvPreset;

	return *this;
}

CgiConfig::~CgiConfig()
{
}

}	 // namespace webserv
