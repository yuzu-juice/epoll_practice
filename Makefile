NAME	= epoll_practice
CXX	= c++
SRCS	= main.cpp
CFLAGS	= -Wall -Werror -Wextra -std=c++98 -pedantic

.DEFAULT:	all

all:
	$(CXX) $(CFLAGS) -o $(NAME) $(SRCS)

.PHONY: all
