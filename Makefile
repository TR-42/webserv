NAME	=	webserv

SRCS_DIR	=	./srcs
OBJS_DIR	=	./objs
DEPS_DIR	=	./deps

OBJS_DEBUG_DIR	=	./objs_debug
DEPS_DEBUG_DIR	=	./deps_debug

SRCS_CONFIG	=\
	CgiConfig.cpp\
	HttpRedirectConfig.cpp\
	HttpRouteConfig.cpp\
	ListenConfig.cpp\
	ServerConfig.cpp\

SRCS_ENV	=\
	EnvManager.cpp\

SRCS_HTTP	=\
	HttpFieldMap.cpp\
	HttpRequest.cpp\
	HttpResponse.cpp\

SRCS_LOGGER	=\
	Logger.cpp\

SRCS_MATH =\
	can_add.cpp\
	can_mul.cpp\

SRCS_SERVICE	=\
	DeleteFileService.cpp\
	GetFileService.cpp\
	pickService.cpp\
	PostFileService.cpp\
	ServiceBase.cpp\
	SimpleService.cpp\

SRCS_SIGNAL	=\
	signal_handler.cpp\

SRCS_SOCKET	=\
	ClientSocket.cpp\
	Poll.cpp\
	ServerSocket.cpp\
	Socket.cpp\

SRCS_UTILS	=\
	ErrorPageProvider.cpp\
	getHttpTimeStr.cpp\
	modeToString.cpp\
	stoul.cpp\
	strtrim.cpp\
	to_string.cpp\
	url_decode.cpp\
	UUID.cpp\
	UUIDv7.cpp\

SRCS_MAIN	=\
	main.cpp\

SRCS_OTHER	=\

SRC_FILES	=\
	$(addprefix config/, $(SRCS_CONFIG))\
	$(addprefix env/, $(SRCS_ENV))\
	$(addprefix http/, $(SRCS_HTTP))\
	$(addprefix logger/, $(SRCS_LOGGER))\
	$(addprefix math/, $(SRCS_MATH))\
	$(addprefix service/, $(SRCS_SERVICE))\
	$(addprefix signal/, $(SRCS_SIGNAL))\
	$(addprefix socket/, $(SRCS_SOCKET))\
	$(addprefix utils/, $(SRCS_UTILS))\
	$(SRCS_MAIN)\
	$(SRCS_OTHER)\

SRC_FILES_DIRNAME_LIST = $(dir $(SRC_FILES))

LINKS	=\

INCLUDES	=\
	-I./headers\

CFLAGS_BASE	=	-Wall -Wextra -Werror -std=c++98 -g
CFLAGS_DEBUG	=	$(CFLAGS_BASE) -O0 -fsanitize=address -DDEBUG -MMD -MP -MF $(DEPS_DEBUG_DIR)/$*.d
CFLAGS	=	$(CFLAGS_BASE) -O2 -MMD -MP -MF $(DEPS_DIR)/$*.d

SRCS	=	$(addprefix $(SRCS_DIR)/, $(SRC_FILES))
OBJS	=	$(addprefix $(OBJS_DIR)/, $(SRC_FILES:.cpp=.o))
DEPS	=	$(addprefix $(DEPS_DIR)/, $(SRC_FILES:.cpp=.d))
OBJS_DEBUG	=	$(addprefix $(OBJS_DEBUG_DIR)/, $(SRC_FILES:.cpp=.o))
DEPS_DEBUG	=	$(addprefix $(DEPS_DEBUG_DIR)/, $(SRC_FILES:.cpp=.d))

# ref: https://qiita.com/dmystk/items/3f82b1eb763c9b9b47e8

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(LINKS) $(CFLAGS) $(INCLUDES) $^ -o $@

# ARGS="abc \"def ghi\"" で実行すると、argv: [0]:'./webserv' [1]:'abc' [2]:'def ghi' になる
leaks: $(NAME)
	@echo "=== Running leaks on $(NAME) ==="
	@leaks -q --atExit -- ./$(NAME) $(ARGS)

debug: $(OBJS_DEBUG)
	$(CXX) $(LINKS) $(CFLAGS_DEBUG) $(INCLUDES) $^ -o $@

$(OBJS_DIR):
	mkdir -p $(addprefix $@/, $(SRC_FILES_DIRNAME_LIST))
$(OBJS_DEBUG_DIR):
	mkdir -p $(addprefix $@/, $(SRC_FILES_DIRNAME_LIST))
$(DEPS_DIR):
	mkdir -p $(addprefix $@/, $(SRC_FILES_DIRNAME_LIST))
$(DEPS_DEBUG_DIR):
	mkdir -p $(addprefix $@/, $(SRC_FILES_DIRNAME_LIST))

dir:
	mkdir -p\
		$(addprefix $(OBJS_DIR)/, $(SRC_FILES_DIRNAME_LIST))\
		$(addprefix $(OBJS_DEBUG_DIR)/, $(SRC_FILES_DIRNAME_LIST))\
		$(addprefix $(DEPS_DIR)/, $(SRC_FILES_DIRNAME_LIST))\
		$(addprefix $(DEPS_DEBUG_DIR)/, $(SRC_FILES_DIRNAME_LIST))

$(DEPS):
$(DEPS_DEBUG):
-include $(DEPS_DEBUG)
-include $(DEPS)

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp $(DEPS_DIR)/%.d | $(OBJS_DIR) $(DEPS_DIR)
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJS_DEBUG_DIR)/%.o: $(SRCS_DIR)/%.cpp $(DEPS_DEBUG_DIR)/%.d | $(OBJS_DEBUG_DIR) $(DEPS_DEBUG_DIR)
	$(CXX) $(CFLAGS_DEBUG) $(INCLUDES) -c $< -o $@

clean: cleanobjs cleanobjs_debug cleandeps cleandir
cleanobjs:
	$(RM) $(OBJS)
cleanobjs_debug:
	$(RM) $(OBJS_DEBUG)
cleandeps:
	$(RM) $(DEPS)
cleandeps_debug:
	$(RM) $(DEPS_DEBUG)
cleandir:
	if [ -d "$(OBJS_DIR)" ] && [ -z `ls -A $(OBJS_DIR)` ]; then rmdir $(OBJS_DIR); fi
	if [ -d "$(OBJS_DEBUG_DIR)" ] && [ -z `ls -A $(OBJS_DEBUG_DIR)` ]; then rmdir $(OBJS_DEBUG_DIR); fi
	if [ -d "$(DEPS_DIR)" ] && [ -z `ls -A $(DEPS_DIR)` ]; then rmdir $(DEPS_DIR); fi
	if [ -d "$(DEPS_DEBUG_DIR)" ] && [ -z `ls -A $(DEPS_DEBUG_DIR)` ]; then rmdir $(DEPS_DEBUG_DIR); fi

cleanf:	cleanobjs cleandeps
	$(RM) -r\
		$(OBJS_DIR)\
		$(OBJS_DEBUG_DIR)\
		$(DEPS_DIR)\
		$(DEPS_DEBUG_DIR)

fclean: clean
	$(RM) $(NAME)
re: fclean all

.PHONY:\
	all\
	leaks\
	clean\
	fclean\
	re\
	dir\
	test\
	cleanobjs\
	cleanobjs_debug\
	cleandeps\
	cleandeps_debug\
	cleandir\
	cleanf\

#region Test Rules

test:
	mkdir -p ./tests_build
	cd ./tests_build && cmake ..
	make -C ./tests_build
	./tests_build/webserv-test --output-on-failure
