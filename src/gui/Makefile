##
## EPITECH PROJECT, 2025
## B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin [WSL: Ubuntu]
## File description:
## Makefile
##

NAME    = ../../zappy_gui

CXX     = g++
CXXFLAGS= -g -O0 -Wall -Wextra -Werror -std=c++17 `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lSDL2_mixer -lSDL2_ttf -lSDL2_image

SRC     = $(shell find src/core src/engine -name '*.cpp')
OBJ_DIR = obj
OBJ     = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))
OBJ_DIRS = $(sort $(dir $(OBJ)))

INC     = -Isrc/core -Isrc/engine

all: $(NAME)

$(NAME): create_dirs $(OBJ)
	$(CXX) $(OBJ) -o $(NAME) $(LDFLAGS)

create_dirs:
	mkdir -p $(OBJ_DIRS)

$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re create_dirs
