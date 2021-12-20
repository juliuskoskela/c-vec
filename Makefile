###############################################################################
#
#	Makefile
#
###############################################################################

NAME		=	libvec.a
UTESTS		=	utests
CC			=	clang
CFLAGS 		=	-O3
WFLAGS		=	-Wall -Wextra -Werror -Wunreachable-code -Wpedantic -Wtype-limits
COMPILE		=	$(CC) $(CFLAGS) $(WFLAGS)

UTESTS_SRC	=	utests.c
SRC			=	vec.c
OBJ			=	vec.o

all:
			@$(COMPILE) -c $(SRC) -o $(OBJ)
			@ar -rcs $(NAME) $(OBJ)
			@echo "compilation successful!"

test: re
			@$(COMPILE) $(UTESTS_SRC) $(NAME) -o $(UTESTS)
			@./utests

clean:
			@rm -rf *.o *.d utests *.out

fclean: clean
			@rm -rf $(NAME);

re: fclean all

.PHONY: all test fclean clean re
