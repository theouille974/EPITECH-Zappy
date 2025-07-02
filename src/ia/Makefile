##
## EPITECH PROJECT, 2025
## B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
## File description:
## Makefile
##

NAME = ../../zappy_ia

PYTHON = python3
SRC = ia.py

all: $(NAME)

$(NAME): $(SRC)
	echo "#!/bin/sh" > $(NAME)
	echo '$(PYTHON) $$(dirname $$0)/src/ia/ia.py "$$@"' >> $(NAME)
	chmod +x $(NAME)

clean:
	rm -f $(NAME)

fclean: clean

re: fclean all

.PHONY: all clean fclean re
