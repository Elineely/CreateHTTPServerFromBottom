# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: kipark <kipark@student.42seoul.kr>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/04/18 14:16:51 by kipark            #+#    #+#              #
#    Updated: 2023/05/12 16:45:03 by kipark           ###   ########seoul.kr   #
#                                                                              #
# **************************************************************************** #

NAME 						= cute_webserv

INCLUDE 				=	-Iinclude/
CXX							=	c++
CXXFLAGS				=	-Wall -Wextra -Werror -std=c++98 -fsanitize=address

SRCS						=	main.cpp \
									server/Server.cpp	\
									config/Config.cpp	\
									config/print.cpp	\
									config/get.cpp

OBJS						=	$(SRCS:.cpp=.o)

all: $(NAME)

$(NAME)		: $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(OBJS) -o $@
	
%.o			: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -g -c $< -o $@

clean:
	rm -rf $(OBJS)

fclean:
	make clean
	rm -rf $(NAME)

re: 
	make fclean
	make all

.PHONY: all clean fclean re
