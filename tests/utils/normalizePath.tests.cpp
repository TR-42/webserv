#include <gtest/gtest.h>

#include <utils/normalizePath.hpp>

TEST(normalizePath, Empty)
{
	EXPECT_EQ("", webserv::utils::normalizePath(""));
}

TEST(normalizePath, Root)
{
	EXPECT_EQ("/", webserv::utils::normalizePath("/"));
}

TEST(normalizePath, RootWithDot)
{
	EXPECT_EQ("/", webserv::utils::normalizePath("/."));
}

TEST(normalizePath, ParentOfRoot)
{
	EXPECT_EQ("/", webserv::utils::normalizePath("/.."));
}

TEST(normalizePath, ParentOfRootWithSlash)
{
	EXPECT_EQ("/", webserv::utils::normalizePath("/../"));
}

TEST(normalizePath, ParentOfRootWithSlashAndDot)
{
	EXPECT_EQ("/", webserv::utils::normalizePath("/../."));
}

TEST(normalizePath, ParentOfRootWithSlashAndDotAndSlash)
{
	EXPECT_EQ("/", webserv::utils::normalizePath("/.././"));
}

TEST(normalizePath, CurrentDirectory)
{
	EXPECT_EQ("", webserv::utils::normalizePath("./"));
	EXPECT_EQ("", webserv::utils::normalizePath("./."));
	EXPECT_EQ("", webserv::utils::normalizePath("././"));
}

TEST(normalizePath, SimpleAbsoluteFilePath)
{
	EXPECT_EQ("/abc/def", webserv::utils::normalizePath("/abc/def"));
}

TEST(normalizePath, SimpleRelativeFilePath)
{
	EXPECT_EQ("abc/def", webserv::utils::normalizePath("abc/def"));
}

TEST(normalizePath, RelativeFilePathFromCurrentDirectory)
{
	EXPECT_EQ("abc/def", webserv::utils::normalizePath("./abc/def"));
}
TEST(normalizePath, RelativeFilePathFromParentDirectory)
{
	EXPECT_EQ("abc/def", webserv::utils::normalizePath("../abc/def"));
}

TEST(normalizePath, DuplicateSlash)
{
	EXPECT_EQ("/abc/def", webserv::utils::normalizePath("/abc//def"));
	EXPECT_EQ("/abc/def", webserv::utils::normalizePath("/abc///def"));
}

TEST(normalizePath, CurrentDirectoryDotInPath)
{
	EXPECT_EQ("/abc/def", webserv::utils::normalizePath("/abc/./def"));
}

TEST(normalizePath, ParentDirectoryDotDotInPath)
{
	EXPECT_EQ("/def", webserv::utils::normalizePath("/abc/../def"));
}
