NAME		:= domginx

CXX			:= g++
CXXFLAGS	:= -std=c++98 -Wall -Wextra -Werror -g3
DEBUGFLAGS	:= -DDEBUG=true -g

SRCDIR		:= src
INCDIR		:= inc
OBJDIR		:= build

SRCS		:= \
				$(SRCDIR)/Client.cpp \
				$(SRCDIR)/Config.cpp \
				$(SRCDIR)/EventLoop.cpp \
				$(SRCDIR)/main.cpp \
				$(SRCDIR)/Server.cpp \
				$(SRCDIR)/ServerConfig.cpp \

OBJS		:= $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
DEPS		:= $(OBJS:.o=.d)

all: $(NAME)
	@echo Starting $(NAME)
	@./$(NAME)

$(NAME): $(OBJS)
	@$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "Built $(NAME)"

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	@$(CXX) $(CXXFLAGS) -I$(INCDIR) -MMD -MP -c $< -o $@

debug: CXXFLAGS += $(DEBUGFLAGS)
debug: re

clean:
	@rm -rf $(OBJDIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re debug