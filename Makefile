NAME 					:= cute_webserv

INCLUDE					:=	-I./include/

SRC_DIR					:=	src/
SRC_MAIN				:=	main.cpp

SRC_CONFIG_DIR			:=	Config/
SRC_CONFIG_FILES		:=	Config.cpp	\
							print.cpp	\
							content.cpp

SRC_PARSER_DIR			:=	Parser/
SRC_PARSER_FILES		:=	Parser.cpp

# SRC_RESPONSE_DIR		:= Response/
# SRC_RESPONSE_FILES		:= Response.cpp

# SRC_HTTPPROCESSOR_DIR		:=	HttpProcessor/
# SRC_HTTPPROCESSOR_FILES		:=	HttpProcessor.cpp

SRC_SERVER_DIR			:=	Server/
SRC_SERVER_FILES		:=	Server.cpp 	\
							socket.cpp	\
							listen.cpp	\
							bind.cpp	\
							kqueue.cpp

SRC_UTILS_DIR			:=	utils/
SRC_UTILS_FILES			:=	ft_config_split.cpp		\
							ft_split.cpp			\
							ft_strdup.cpp			\
							ft_strjoin.cpp			\
							ft_strlen.cpp			\
							ft_strtrim.cpp			\
							ft_toLower.cpp			\
							ft_error.cpp			\
							ft_process_print.cpp

SRC_FILES				:=	$(SRC_MAIN)	\
							$(addprefix $(SRC_INIT_DIR), $(SRC_INIT_FILES))		\
							$(addprefix $(SRC_CONFIG_DIR), $(SRC_CONFIG_FILES))	\
							$(addprefix $(SRC_PARSER_DIR), $(SRC_PARSER_FILES))	\
							$(addprefix $(SRC_HTTPPROCESSOR_DIR), $(SRC_HTTPPROCESSOR_FILES)) \
							$(addprefix $(SRC_SERVER_DIR), $(SRC_SERVER_FILES))	\
							$(addprefix $(SRC_RESPONSE_DIR), $(SRC_RESPONSE_FILES))	\
							$(addprefix $(SRC_UTILS_DIR), $(SRC_UTILS_FILES))
SRCS				:=	$(addprefix $(SRC_DIR), $(SRC_FILES))

OBJ_DIR				:=	obj/
OBJS				:=	$(SRCS:%.cpp=$(OBJ_DIR)%.o)

ifdef DEBUG_MODE
	CXXFLAGS					:=	$(CXXFLAGS) -g3
endif

ifdef D_SANI
	CXXFLAGS					:=	$(CXXFLAGS) -g3 -fsanitize=address
endif

.PHONY : all
all : $(NAME)

.PHONY : clean
clean :
	rm -rf $(OBJ_DIR)

.PHONY : fclean
fclean : clean
	rm -f $(NAME)

.PHONY : re
re : fclean
	make -j4 all

.PHONY : debug
debug : fclean
	make -j4 DEBUG_MODE=1 all

.PHONY : dsani
dsani : fclean
	make -j4 D_SANI=1 all

$(NAME) : $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJS): $(OBJ_DIR)%.o: %.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDE)

