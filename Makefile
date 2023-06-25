NAME 								:= cute_webserv

INCLUDE								:=	-I./include/

SRC_DIR								:=	src/
SRC_MAIN							:=	main.cpp

SRC_CONFIG_DIR						:=	Config/
SRC_CONFIG_FILES					:=	Config.cpp	\
										print.cpp	\
										content.cpp

SRC_HTTPPROCESSOR_DIR				:=	HttpProcessor/
SRC_HTTPPROCESSOR_FILES				:=	HttpProcessor.cpp \
										MethodHandler.cpp \
										PathFinder.cpp

SRC_CGI_HANDLER_DIR					:=	CgiHandler/
SRC_CGI_HANDLER_FILES				:=	CgiHandler.cpp 		\
										GetCgiHandler.cpp	\
										PostCgiHandler.cpp

SRC_PARSER_DIR						:=	Parser/
SRC_PARSER_FILES					:=	Parser.cpp				\
										parseHeaders.cpp		\
										parseFirstLine.cpp		\
										parseBody.cpp			\
										parseChunkedBody.cpp

SRC_RESPONSE_GENERATOR_DIR			:= ResponseGenerator/
SRC_RESPONSE_GENERATOR_FILES		:=	Mime.cpp \
										ResponseGenerator.cpp \
										StatusCode.cpp

SRC_SERVER_FINDER_DIR				:= ServerFinder/
SRC_SERVER_FINDER_FILES				:= ServerFinder.cpp

SRC_SERVER_DIR			:=	Server/
SRC_SERVER_FILES		:=	Server.cpp 				\
							socket/socket.cpp		\
							socket/listen.cpp		\
							socket/bind.cpp			\
							socket/kqueue.cpp		\
							socket/setServers.cpp 	\
							socket/accept.cpp		\
							event/client.cpp		\
							event/pipe.cpp			\
							event/process.cpp		\
							event/server.cpp		\
							event/static_file.cpp	\
							event/udata.cpp

SRC_UTILS_DIR			:=	utils/
SRC_UTILS_FILES			:=	ft_config_split.cpp		\
							ft_split.cpp			\
							ft_strtrim.cpp			\
							ft_toLower.cpp			\
							ft_error.cpp			\
							ft_process_print.cpp

SRC_LOG_DIR				:=	Log/
SRC_LOG_FILES			:=	Log.cpp

SRC_FILES				:=	$(SRC_MAIN)	\
							$(addprefix $(SRC_INIT_DIR), $(SRC_INIT_FILES))		\
							$(addprefix $(SRC_CONFIG_DIR), $(SRC_CONFIG_FILES))	\
							$(addprefix $(SRC_PARSER_DIR), $(SRC_PARSER_FILES))	\
							$(addprefix $(SRC_HTTPPROCESSOR_DIR), $(SRC_HTTPPROCESSOR_FILES)) \
							$(addprefix $(SRC_CGI_HANDLER_DIR), $(SRC_CGI_HANDLER_FILES)) \
							$(addprefix $(SRC_SERVER_DIR), $(SRC_SERVER_FILES))	\
							$(addprefix $(SRC_RESPONSE_GENERATOR_DIR), $(SRC_RESPONSE_GENERATOR_FILES))	\
							$(addprefix $(SRC_UTILS_DIR), $(SRC_UTILS_FILES)) \
							$(addprefix $(SRC_LOG_DIR), $(SRC_LOG_FILES)) \
							$(addprefix $(SRC_SERVER_FINDER_DIR), $(SRC_SERVER_FINDER_FILES))

SRCS					:=	$(addprefix $(SRC_DIR), $(SRC_FILES))

OBJ_DIR					:=	obj/
OBJS					:=	$(SRCS:%.cpp=$(OBJ_DIR)%.o)

CXXFLAGS				:=	-Wall -Werror -Wextra -std=c++98 -pedantic

ifdef DEBUG_MODE
	CXXFLAGS			:=	$(CXXFLAGS) -g3
endif

ifdef D_SANI
	CXXFLAGS			:=	$(CXXFLAGS) -g3 -fsanitize=address
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

