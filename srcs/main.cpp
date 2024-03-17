#include <Logger.hpp>
#include <iostream>

class mainClass
{
 private:
	webserv::Logger logger;

 public:
	void testLogger(void);
};

void mainClass::testLogger()
{
	C_LOG("Hello, World!");
	C_DEBUG("Hello, World!");
	C_INFO("Hello, World!");
	C_WARN("Hello, World!");
	C_ERROR("Hello, World!");
	C_FATAL("Hello, World!");

	C_LOG() << "Hello, Stream World!" << std::endl;
	C_DEBUG() << "Hello, Stream World!" << std::endl;
	C_INFO() << "Hello, Stream World!" << std::endl;
	C_WARN() << "Hello, Stream World!" << std::endl;
	C_ERROR() << "Hello, Stream World!" << std::endl;
	C_FATAL() << "Hello, Stream World!" << std::endl;
}

static std::string get_argv_str(int argc, const char *argv[])
{
	std::string str;
	for (int i = 0; i < argc; i++) {
		if (i != 0) {
			str += " ";
		}
		str += "[" + std::to_string(i) + "]:'" + argv[i] + "'";
	}
	return str;
}

int main(int argc, const char *argv[])
{
	webserv::Logger logger;

	std::cout << "Hello, World!" << std::endl;
	L_LOG("argv: " + get_argv_str(argc, argv));

	L_LOG("Hello, World!");
	L_DEBUG("Hello, World!");
	L_INFO("Hello, World!");
	L_WARN("Hello, World!");
	L_ERROR("Hello, World!");
	L_FATAL("Hello, World!");
	L_LOG() << "Hello, Stream World!" << std::endl;
	L_DEBUG() << "Hello, Stream World!" << std::endl;
	L_INFO() << "Hello, Stream World!" << std::endl;
	L_WARN() << "Hello, Stream World!" << std::endl;
	L_ERROR() << "Hello, Stream World!" << std::endl;
	L_FATAL() << "Hello, Stream World!" << std::endl;
	mainClass().testLogger();
	return 0;
}
