/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** egg
*/

#include <stdbool.h>

#ifndef EGG_H
    #define EGG_H
    #define EGG_MAX 1024

struct s_net;

/**
 * @brief Structure representing an egg in the game.
 * @note This structure holds the ID, position (x, y), and team index of the egg.
 * @note It is used to manage eggs created by players in the game world.
 */
typedef struct s_egg {
    int id;
    int x;
    int y;
    int team_idx;
} egg_t;

/**
 * @brief Creates an egg at the specified position for the given team.
 * @param net Pointer to the network structure.
 * @param x The X coordinate where the egg will be created.
 * @param y The Y coordinate where the egg will be created.
 * @param team_idx The index of the team that owns the egg.
 * @return True if the egg was successfully created, false otherwise.
 * @note This function initializes a new egg and adds it to the game world.
 */
bool egg_create(struct s_net *net, int x, int y, int team_idx);

/**
 * @brief Hatches an egg for the specified team.
 * @param net Pointer to the network structure.
 * @param team_idx The index of the team that owns the egg.
 * @param x Pointer to store the X coordinate of the hatched egg.
 * @param y Pointer to store the Y coordinate of the hatched egg.
 * @return True if the egg was successfully hatched, false otherwise.
 * @note This function removes the egg from the game world and returns its position.
 */
bool egg_hatch(struct s_net *net, int team_idx, int *x, int *y);

#endif /* EGG_H */
