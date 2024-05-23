#include <fcntl.h>
#include <sys/stat.h>

#include <utils/modeToString.hpp>

namespace webserv
{

namespace utils
{

std::string modeToString(mode_t mode)
{
	char buf[11];

	buf[0] = S_ISDIR(mode) ? 'd' : '-';
	buf[1] = (mode & S_IRUSR) ? 'r' : '-';
	buf[2] = (mode & S_IWUSR) ? 'w' : '-';
	buf[3] = (mode & S_IXUSR) ? 'x' : '-';
	buf[4] = (mode & S_IRGRP) ? 'r' : '-';
	buf[5] = (mode & S_IWGRP) ? 'w' : '-';
	buf[6] = (mode & S_IXGRP) ? 'x' : '-';
	buf[7] = (mode & S_IROTH) ? 'r' : '-';
	buf[8] = (mode & S_IWOTH) ? 'w' : '-';
	buf[9] = (mode & S_IXOTH) ? 'x' : '-';
	buf[10] = '\0';

	return std::string(buf);
}

}	 // namespace utils

}	 // namespace webserv
