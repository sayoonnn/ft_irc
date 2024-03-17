
NAME	=	ircserv

SRC		=	main.cpp\
			Server.cpp\
			Client.cpp

OBJDIR	=	.objs
OBJ		=	$(SRC:%.cpp=$(OBJDIR)/%.o)

INC		=	include

CXX			= c++
CXXFLAGS	= -Wall -Werror -Wextra -std=c++98

vpath %.cpp	src
vpath %.hpp	include

all :
	@make $(NAME)

$(NAME): $(OBJ) 
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
