#include <config/CgiConfig.hpp>

namespace webserv
{

CgiConfig::CgiConfig()
{
}

CgiConfig::CgiConfig(
	const std::string &extensionWithoutDot,
	const std::string &cgiExecutableFullPath
) : _ExtensionWithoutDot(extensionWithoutDot),
		_CgiExecutableFullPath(cgiExecutableFullPath)
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
		from.getCgiExecutableFullPath()
	);

	return *this;
}

void CgiConfig::setProps(
	const std::string &extensionWithoutDot,
	const std::string &cgiExecutableFullPath
)
{
	this->_ExtensionWithoutDot = extensionWithoutDot;
	this->_CgiExecutableFullPath = cgiExecutableFullPath;
}

CgiConfig::~CgiConfig()
{
}

IMPL_REF_GETTER_SETTER_NS(std::string, ExtensionWithoutDot, CgiConfig::)
IMPL_REF_GETTER_SETTER_NS(std::string, CgiExecutableFullPath, CgiConfig::)

}	 // namespace webserv
