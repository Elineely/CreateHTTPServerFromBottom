# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: kipark <kipark@student.42seoul.kr>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/04/18 14:16:51 by kipark            #+#    #+#              #
#    Updated: 2023/05/10 20:20:47 by kipark           ###   ########seoul.kr   #
#                                                                              #
# **************************************************************************** #

NAME 						= 	static_webserv

CXX							=	c++
CXXFLAGS				=	-Wall -Wextra -Werror -std=c++98 -fsanitize=address

SRCS						=	main.cpp \
									server/Server.cpp	\
									parse/Parser.cpp

OBJS						=	$(SRCS:.cpp=.o)

all: $(NAME)

$(NAME)		: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@
	
%.o			: %.cpp
	$(CC) $(CFLAGS) -g -c $< -o $@

clean:
	rm -rf $(OBJS)

fclean:
	make clean
	rm -rf $(NAME)

re: 
	make fclean
	make all

.PHONY: all clean fclean re
