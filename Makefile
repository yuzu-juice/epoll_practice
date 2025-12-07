NAME	= epoll_practice
CXX	= c++
SRCS	= main.cpp
CFLAGS	= -std=c++98

all:
	$(CXX) $(CFLAGS) -o $(NAME) $(SRCS)

.PHONY: all
