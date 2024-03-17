#include <gtest/gtest.h>

#include <EnvManager.hpp>

TEST(EnvManager, envp_null)
{
	EXPECT_EQ(0, webserv::env::EnvManager(NULL).size());
	EXPECT_EQ(0, webserv::env::EnvManager(nullptr).size());
};

TEST(EnvManager, envp_empty_envp)
{
	const char *envp[] = {NULL};
	EXPECT_EQ(0, webserv::env::EnvManager(envp).size());
};

TEST(EnvManager, envp_empty_kvp)
{
	const char *envp[] = {"", NULL};
	EXPECT_EQ(0, webserv::env::EnvManager(envp).size());
};

TEST(EnvManager, envp_one)
{
	const char *envp[] = {"key=value", NULL};
	EXPECT_EQ(1, webserv::env::EnvManager(envp).size());
	EXPECT_STREQ("value", webserv::env::EnvManager(envp).get("key").c_str());
};

TEST(EnvManager, envp_two)
{
	const char *envp[] = {"key1=value1", "key2=value2", NULL};
	EXPECT_EQ(2, webserv::env::EnvManager(envp).size());
	EXPECT_STREQ("value1", webserv::env::EnvManager(envp).get("key1").c_str());
	EXPECT_STREQ("value2", webserv::env::EnvManager(envp).get("key2").c_str());
};

TEST(EnvManager, envp_duplicate)
{
	const char *envp[] = {"key=value1", "key=value2", NULL};
	EXPECT_EQ(1, webserv::env::EnvManager(envp).size());
	EXPECT_STREQ("value2", webserv::env::EnvManager(envp).get("key").c_str());
};

TEST(EnvManager, envp_invalid)
{
	const char *envp[] = {"key1", "key2=value", "key3=value=value", "key4=value=", NULL};
	EXPECT_EQ(3, webserv::env::EnvManager(envp).size());
	EXPECT_THROW(webserv::env::EnvManager(envp).get("key1"), std::out_of_range);
	EXPECT_STREQ("value", webserv::env::EnvManager(envp).get("key2").c_str());
	EXPECT_STREQ("value=value", webserv::env::EnvManager(envp).get("key3").c_str());
	EXPECT_STREQ("value=", webserv::env::EnvManager(envp).get("key4").c_str());
}

TEST(EnvManager, set)
{
	webserv::env::EnvManager env(NULL);
	env.set("key", "value");
	EXPECT_EQ(1, env.size());
	EXPECT_STREQ("value", env.get("key").c_str());
	env.set("key", "value2");
	EXPECT_EQ(1, env.size());
	EXPECT_STREQ("value2", env.get("key").c_str());
};

TEST(EnvManager, operator_brackets)
{
	webserv::env::EnvManager env(NULL);
	env["key"] = "value";
	EXPECT_EQ(1, env.size());
	EXPECT_STREQ("value", env.get("key").c_str());
	env["key"] = "value2";
	EXPECT_EQ(1, env.size());
	EXPECT_STREQ("value2", env.get("key").c_str());
};

TEST(EnvManager, toEnvp)
{
	webserv::env::EnvManager env(NULL);

	env["key1"] = "value1";
	env["key2"] = "value2";

	char **envp = env.toEnvp();
	EXPECT_STREQ("key1=value1", envp[0]);
	EXPECT_STREQ("key2=value2", envp[1]);
	EXPECT_EQ(NULL, envp[2]);
	delete[] envp[0];
	delete[] envp[1];
	delete[] envp;
};

TEST(EnvManager, toEnvp_empty)
{
	webserv::env::EnvManager env(NULL);

	char **envp = env.toEnvp();
	EXPECT_EQ(NULL, envp[0]);
	delete[] envp;
};

TEST(EnvManager, environ)
{
	webserv::env::EnvManager env;

	EXPECT_NE(0, env.size());
};
