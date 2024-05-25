#include <gtest/gtest.h>

#include <utils/split.hpp>

using namespace webserv::utils;

TEST(utils_split, empty_input)
{
	std::string str = "";
	char delim = ',';
	std::vector<std::string> result = split(str, delim);

	ASSERT_EQ(result.size(), 0);
}

TEST(utils_split, empty_delim)
{
	std::string str = "a,b,c,d,e";
	std::string delim = "";
	std::vector<std::string> result = split(str, delim);

	ASSERT_EQ(result.size(), 1);
	ASSERT_EQ(result[0], "a,b,c,d,e");
}

TEST(utils_split, delim_null_char)
{
	std::string str = "a,b,c,d,e";
	char delim = '\0';
	std::vector<std::string> result = split(str, delim);

	ASSERT_EQ(result.size(), 1);
	ASSERT_EQ(result[0], "a,b,c,d,e");
}

TEST(utils_split, split_string_by_char)
{
	std::string str = "a,b,c,d,e";
	char delim = ',';
	std::vector<std::string> result = split(str, delim);

	ASSERT_EQ(result.size(), 5);
	ASSERT_EQ(result[0], "a");
	ASSERT_EQ(result[1], "b");
	ASSERT_EQ(result[2], "c");
	ASSERT_EQ(result[3], "d");
	ASSERT_EQ(result[4], "e");
}

TEST(utils_split, split_string_by_string)
{
	std::string str = "a, b, c, d, e";
	std::string delim = ", ";
	std::vector<std::string> result = split(str, delim);

	ASSERT_EQ(result.size(), 5);
	ASSERT_EQ(result[0], "a");
	ASSERT_EQ(result[1], "b");
	ASSERT_EQ(result[2], "c");
	ASSERT_EQ(result[3], "d");
	ASSERT_EQ(result[4], "e");
}

TEST(utils_split, split_string_by_char_delim_repeat)
{
	std::string str = "a,,b,,c,,d,,e";
	char delim = ',';
	std::vector<std::string> result = split(str, delim);

	ASSERT_EQ(result.size(), 5);
	ASSERT_EQ(result[0], "a");
	ASSERT_EQ(result[1], "b");
	ASSERT_EQ(result[2], "c");
	ASSERT_EQ(result[3], "d");
	ASSERT_EQ(result[4], "e");
}

TEST(utils_split, path_0)
{
	std::string str = "/";
	char delim = '/';
	std::vector<std::string> result = split(str, delim);

	ASSERT_EQ(result.size(), 0);
}

TEST(utils_split, path_1)
{
	std::string str = "/abc";
	char delim = '/';
	std::vector<std::string> result = split(str, delim);

	ASSERT_EQ(result.size(), 1);
	ASSERT_EQ(result[0], "abc");
}

TEST(utils_split, path_2)
{
	std::string str = "/abc/def";
	char delim = '/';
	std::vector<std::string> result = split(str, delim);

	ASSERT_EQ(result.size(), 2);
	ASSERT_EQ(result[0], "abc");
	ASSERT_EQ(result[1], "def");
}
