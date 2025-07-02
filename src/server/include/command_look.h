/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** gui.h
*/

#include <stddef.h>
#include <stdbool.h>

#ifndef COMMAND_LOOK_H
    #define COMMAND_LOOK_H

struct s_player;
struct s_net;

/**
 * @brief Context structure for building a buffer for the "Look" command.
 * @note This structure holds the destination buffer, remaining size, and a flag indicating if a space is needed.
 * @note It is used to manage the output buffer while constructing the view of the player's surroundings.
 */
typedef struct s_buf_ctx {
    char *dst;
    size_t rem;
    bool *need_sp;
} buf_ctx_t;

/**
 * @brief Context structure for viewing the player's surroundings.
 * @note This structure holds the buffer, its size, a pointer to the length of the buffer, and a flag indicating if it's the first view.
 * @note It is used to manage the output while traversing the player's view in the game world.
 */
typedef struct s_view_ctx {
    char *buf;
    size_t size;
    size_t *len;
    bool *first;
} view_ctx_t;

/**
 * @brief Structure representing a position in the game world.
 * @note This structure holds the X and Y coordinates of a position.
 */
typedef struct s_pos {
    int x;
    int y;
} pos_t;

/**
 * @brief Composes the "Look" command output for a player.
 * @param p Pointer to the player structure.
 * @param buf Buffer to store the composed output.
 * @param size Size of the buffer.
 * @return The number of bytes written to the buffer.
 * @note This function generates the view of the player's surroundings and formats it into a string.
 */
void cmd_look(struct s_player *p);

/**
 * @brief Wraps a coordinate value to ensure it stays within the bounds of the game world.
 * @param v The coordinate value to wrap.
 * @param max The maximum value for the coordinate (e.g., width or height of the game world).
 * @return The wrapped coordinate value.
 * @note This function ensures that the coordinate value wraps around when it exceeds the maximum bounds.
 */
int wrap_coord(int v, int max);

/**
 * @brief Builds a tile representation for a net at a specific position.
 * @param net Pointer to the net structure.
 * @param x The X coordinate of the tile.
 * @param y The Y coordinate of the tile.
 * @param b Pointer to the buffer context for building the tile.
 * @return The size of the built tile in bytes.
 * @note This function constructs a string representation of a tile based on the net's properties and position.
 */
size_t build_tile(const struct s_net *net, int x, int y,
    buf_ctx_t *b);

/**
 * @brief Coordinates a position based on the player's direction and distance.
 * @param pl Pointer to the player structure.
 * @param dist The distance to move in the specified direction.
 * @param off The offset to apply to the position.
 * @param pos Pointer to the position structure to store the result.
 * @note This function calculates the new position based on the player's direction and distance, applying the
 * @note offset to the resulting coordinates.
 */
void coord_for_dir(const struct s_player *pl, int dist, int off,
    pos_t *pos);

/**
 * @brief Appends a tile representation to the view context.
 * @param pl Pointer to the player structure.
 * @param tx The X coordinate of the tile.
 * @param ty The Y coordinate of the tile.
 * @param v Pointer to the view context to which the tile will be appended.
 * @note This function adds a tile representation to the view context, managing the output buffer and
 * @note ensuring proper formatting.
 */
void append_tile(const struct s_player *pl, int tx, int ty,
    view_ctx_t *v);

/**
 * @brief Adds a line to the view context for a player.
 * @param pl Pointer to the player structure.
 * @param dist The distance from the player to the line.
 * @param v Pointer to the view context to which the line will be added.
 * @note This function constructs a line of sight for the player, adding it to the view context.
 * @note It manages the output buffer and ensures proper formatting of the line.
 */
void add_line(const struct s_player *pl, int dist, view_ctx_t *v);

/**
 * @brief Traverses the player's view and builds the output buffer.
 * @param pl Pointer to the player structure.
 * @param v Pointer to the view context containing the output buffer and its properties.
 * @note This function iterates through the player's surroundings, building the output buffer with tile representations
 * @note and lines of sight, managing the formatting and spacing as needed.
 */
void traverse_view(const struct s_player *pl, view_ctx_t *v);

#endif /* COMMAND_LOOK_H */
