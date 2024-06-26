# ref: https://komorinfo.com/blog/add-gtest-and-gmock-into-your-project/
cmake_minimum_required(VERSION 3.13)
project(webserv-test LANGUAGES CXX)

set(PROGRAM webserv-test)
SET(CMAKE_CXX_FLAGS "-fsanitize=address -g")

# Google Testの不要なキャッシュ変数をオフにしておく
option(BUILD_GMOCK "Builds the googlemock subprojects" OFF)
option(INSTALL_GTEST "Enables installation of googletest" OFF)

# Google Testをこのプロジェクトに組み込む
add_subdirectory(googletest)

add_definitions(-DGTEST)

file(GLOB TEST_SOURCES
	tests/*/*.cpp)

file(GLOB SOURCES
	srcs/*/*.cpp)

add_executable(${PROGRAM}
	${SOURCES}
	${TEST_SOURCES}
)

target_link_libraries(${PROGRAM}
	PRIVATE

	# Google Testをリンクする
	gtest
	gtest_main
)

target_include_directories(${PROGRAM}
	PUBLIC
	headers
	googletest/googletest/include
)

enable_testing()
add_test(NAME WebServTest COMMAND ${PROGRAM})
