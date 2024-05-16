#include <config/CgiConfig.hpp>

namespace webserv
{

CgiConfig::CgiConfig()
{
}

CgiConfig::CgiConfig(
	const std::string &extensionWithoutDot,
	const std::string &cgiExecutableFullPath,
	const env::EnvManager &envPreset
) : _ExtensionWithoutDot(extensionWithoutDot),
		_CgiExecutableFullPath(cgiExecutableFullPath),
		_EnvPreset(envPreset)
{
}

CgiConfig::CgiConfig(
	const CgiConfig &from
)
{
	*this = from;
}

CgiConfig &CgiConfig::operator=(
	const CgiConfig &from
)
{
	if (this == &from)
		return *this;

	this->setProps(
		from.getExtensionWithoutDot(),
		from.getCgiExecutableFullPath(),
		from.getEnvPreset()
	);

	return *this;
}

void CgiConfig::setProps(
	const std::string &extensionWithoutDot,
	const std::string &cgiExecutableFullPath,
	const env::EnvManager &envPreset
)
{
	this->_ExtensionWithoutDot = extensionWithoutDot;
	this->_CgiExecutableFullPath = cgiExecutableFullPath;
	this->_EnvPreset = envPreset;
}

CgiConfig::~CgiConfig()
{
}

IMPL_REF_GETTER_SETTER_NS(std::string, ExtensionWithoutDot, CgiConfig::)
IMPL_REF_GETTER_SETTER_NS(std::string, CgiExecutableFullPath, CgiConfig::)
IMPL_REF_GETTER_SETTER_NS(env::EnvManager, EnvPreset, CgiConfig::)

}	 // namespace webserv
