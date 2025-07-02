##
## EPITECH PROJECT, 2025
## B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
## File description:
## Makefile
##

CC      = gcc
CFLAGS  = -Wall -Wextra -Werror -pedantic -std=c17
INCS    = -Iinclude

NAME    = ../../zappy_server
SRC     = $(wildcard src/*.c)
OBJ     = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
