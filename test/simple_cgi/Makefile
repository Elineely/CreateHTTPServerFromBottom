NAME 				:= cgi

SRCS				:= main.cpp

OBJ_DIR				:=	obj/
OBJS				:=	$(SRCS:%.cpp=$(OBJ_DIR)%.o)

ifdef DEBUG_MODE
	CFLAGS					:=	$(CFLAGS) -g
endif

ifdef D_SANI
	CFLAGS					:=	$(CFLAGS) -g -fsanitize=address
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
re : fclean all

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

