
NAME	=	ircserv

SRC		=	main.cpp\
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

OBJDIR	=	.objs
OBJ		=	$(SRC:%.cpp=$(OBJDIR)/%.o)

INC		=	include

CXX			= c++
CXXFLAGS	= -Wall -Werror -Wextra -std=c++98

vpath %.cpp	src
vpath %.hpp	include

all :
	@make $(NAME)

$(NAME): $(OBJ) $(INCLUDES)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)
	@echo $(NAME) DONE ✅ 

$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	@$(CXX) $(CXXFLAGS) -I $(INC) -c $< -o $@

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
