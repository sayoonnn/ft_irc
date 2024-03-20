
NAME		=	ircserv

SRC			=	main.cpp\
				Server.cpp\
				ServerKqueue.cpp\
				ServerNetwork.cpp\
				ServerCommands.cpp\
				Client.cpp

INCLUDES	=	Client.hpp\
				Command.hpp\
				RPL.hpp\
				Server.hpp\
				Types.hpp

OBJDIR		=	.objs
OBJ			=	$(SRC:%.cpp=$(OBJDIR)/%.o)

INC			=	include

CXX			= c++
CXXFLAGS	= -Wall -Werror -Wextra -std=c++98

vpath %.cpp	src
vpath %.hpp	include

UP			=	\033[A
CUT			=	\033[K
YELLOW		=	\033[1;33m
RESET		=	\033[0m

all :
	@make $(NAME)

$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)
	@echo $(NAME) DONE ✅ 

$(OBJDIR)/%.o: %.cpp  $(INCLUDES) | $(OBJDIR)
	@echo "$(YELLOW)compiling [$@]... $(RESET)"
	@$(CXX) $(CXXFLAGS) -I $(INC) -c $< -o $@
	@printf "$(UP)$(CUT)"

$(OBJDIR):
	@mkdir $(OBJDIR)

clean:
	@rm -rf $(OBJDIR)
	@echo CLEAN DONE ✅

fclean:
	@make clean
	@rm -f $(NAME)
	@echo FCLEAN DONE ✅

re: 
	@make fclean
	@make all

.PHONY:	all bonus clean fclean re
