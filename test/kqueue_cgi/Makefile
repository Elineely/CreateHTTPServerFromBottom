NAME 					:= kqueue_cgi

INCLUDE					:=	-I./include/

SRC_MAIN				:=	main.cpp

SRC_FILES				:=	$(SRC_MAIN)
SRCS					:=	$(SRC_FILES)

OBJ_DIR					:=	obj/
OBJS					:=	$(SRCS:%.cpp=$(OBJ_DIR)%.o)

ifdef DEBUG_MODE
	CXXFLAGS				:=	$(CXXFLAGS) -g3
endif

ifdef D_SANI
	CXXFLAGS				:=	$(CXXFLAGS) -g3 -fsanitize=address
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

