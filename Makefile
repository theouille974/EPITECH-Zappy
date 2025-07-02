##
## EPITECH PROJECT, 2025
## B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin [WSL: Ubuntu]
## File description:
## Makefile
##

all: zappy_server zappy_gui zappy_ia

zappy_server:
	$(MAKE) -C src/server

zappy_gui:
	$(MAKE) -C src/gui

zappy_ia:
	$(MAKE) -C src/ia

clean:
	$(MAKE) -C src/server clean
	$(MAKE) -C src/gui clean
	$(MAKE) -C src/ia clean

fclean:
	$(MAKE) -C src/server fclean
	$(MAKE) -C src/gui fclean
	$(MAKE) -C src/ia fclean

re:	fclean all

test: all
	@pytest tests/test_server.py

test_exec:
	@pytest tests/test_server.py

.PHONY: all clean fclean re zappy_server zappy_gui test zappy_ia
