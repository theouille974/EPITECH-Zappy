/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** incantation_utils.h
*/

#include <stdint.h>
#include <stdbool.h>
#include "world.h"
#include "player.h"
#include "net_poll.h"

#ifndef INCANTATION_UTILS_H
    #define INCANTATION_UTILS_H

/**
 * @brief Gets the current time in milliseconds.
 * @return The current time in milliseconds since the epoch.
 */
uint64_t inc_now_ms(void);
/**
 * @brief Increments the count of players at a specific tile and level.
 * @param net Pointer to the network structure containing the game state.
 * @param x The X coordinate of the tile.
 * @param y The Y coordinate of the tile.
 * @param lvl The level of the incantation.
 * @return The count of players at the specified tile and level.
 */
int inc_count_players(const net_t *net, int x, int y, int lvl);
/**
 * @brief Checks if the resources on a tile are sufficient for an incantation.
 * @param t Pointer to the tile structure.
 * @param stones Array of required resources for the incantation.
 * @return True if resources are sufficient, false otherwise.
 */
bool inc_resources_sufficient(const tile_t *t, const int stones[RES_MAX]);
/**
 * @brief Consumes resources from a tile for an incantation.
 * @param t Pointer to the tile structure.
 * @param stones Array of resources to consume.
 */
void inc_consume_resources(tile_t *t, const int stones[RES_MAX]);
/**
 * @brief Increments the level of players at a specific tile and level.
 * @param net Pointer to the network structure containing the game state.
 * @param x The X coordinate of the tile.
 * @param y The Y coordinate of the tile.
 * @param lvl The level to increment players to.
 */
void inc_level_up_players(net_t *net, int x, int y, int lvl);

#endif /* INCANTATION_UTILS_H */
