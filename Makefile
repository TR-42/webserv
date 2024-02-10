NAME	=	webserv

SRCS_DIR	=	./srcs
OBJS_DIR	=	./objs
DEPS_DIR	=	./deps

SRCS_MAIN	=\
	main.cpp\

SRCS_OTHER	=\

SRC_FILES	=\
	$(SRCS_MAIN)\
	$(SRCS_OTHER)\

LINKS	=\

INCLUDES	=\
	-I./headers\

CFLAGS	=	-Wall -Wextra -Werror -std=c++98 -fsanitize=address -g -MMD -MP -MF $(DEPS_DIR)/$*.d

SRCS	=	$(addprefix $(SRCS_DIR)/, $(SRC_FILES))
OBJS	=	$(addprefix $(OBJS_DIR)/, $(SRC_FILES:.cpp=.o))
DEPS	=	$(addprefix $(DEPS_DIR)/, $(SRC_FILES:.cpp=.d))

# ref: https://qiita.com/dmystk/items/3f82b1eb763c9b9b47e8

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(LINKS) $(CFLAGS) $(INCLUDES) $^ -o $@

$(OBJS_DIR):
	@mkdir -p $@
$(DEPS_DIR):
	@mkdir -p $@

$(DEPS):
-include $(DEPS)

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp $(DEPS_DIR)/%.d | $(OBJS_DIR) $(DEPS_DIR)
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean: cleanobjs cleandeps cleandir
cleanobjs:
	$(RM) $(OBJS)
cleandeps:
	$(RM) $(DEPS)
cleandir:
	if [ -z "$(shell ls -A $(OBJS_DIR))" ]; then rmdir $(OBJS_DIR); fi
	if [ -z "$(shell ls -A $(DEPS_DIR))" ]; then rmdir $(DEPS_DIR); fi
cleanf:	cleanobjs cleandeps
	$(RM) -r $(OBJS_DIR) $(DEPS_DIR)

fclean: clean
	$(RM) $(NAME)
re: fclean all

.PHONY: all clean fclean re