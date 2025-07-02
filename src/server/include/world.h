/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** world
*/

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "cfg.h"

#ifndef WORLD_H
    #define WORLD_H

/**
 * @brief Structure representing a tile in the game world.
 * @param res An array of resource counts for each type of resource on the tile.
 * @note This structure is used to represent the resources available on a specific tile in the game world.
 * @note Each tile can contain different types of resources, and this structure allows for easy
 *       management and access to those resources.
 */
typedef enum e_res {
    RES_FOOD,
    RES_LINEMATE,
    RES_DERAUMERE,
    RES_SIBUR,
    RES_MENDIANE,
    RES_PHIRAS,
    RES_THYSTAME,
    RES_MAX
} res_t;

/**
 * @brief Structure representing the configuration of resources in the game world.
 * @param res An array of resource counts for each type of resource.
 * @note This structure is used to define the initial state of resources in the game world.
 */
typedef struct s_tile {
    uint16_t res[RES_MAX];
} tile_t;

/**
 * @brief Structure representing the game world.
 * @param w The width of the world in tiles.
 * @param h The height of the world in tiles.
 * @param tiles An array of tiles representing the game world.
 * @note This structure encapsulates the entire game world, including its dimensions and the resources available on each tile.
 * @note It is used to manage the state of the game world and facilitate interactions between players and resources.
 */
typedef struct s_world {
    int w;
    int h;
    tile_t *tiles;
} world_t;

/**
 * @brief Initializes the game world based on the provided configuration.
 * @param w Pointer to the world structure to be initialized.
 * @param cfg Pointer to the configuration structure containing world settings.
 * @return True if the world was successfully created, false otherwise.
 * @note This function sets up the game world according to the specified configuration, including dimensions and initial resources.
 */
bool world_create(world_t *w, const cfg_t *cfg);
/**
 * @brief Destroys the game world, freeing any allocated resources.
 * @param w Pointer to the world structure to be destroyed.
 * @note This function cleans up the game world, releasing memory and resources associated with it.
 */
void world_destroy(world_t *w);
/**
 * @brief Respawns the game world, resetting resources and tiles.
 * @param w Pointer to the world structure to be respawned.
 * @note This function resets the state of the game world, making it ready for a new game session.
 */
void world_respawn(world_t *w);
/**
 * @brief Periodically refills resources in the game world.
 * @param w Pointer to the world structure where resources will be refilled.
 * @note This function is called at regular intervals to replenish resources in the game world,
 *       ensuring that players have access to resources throughout the game.
 */
void world_periodic_refill(world_t *w);
/**
 * @brief Converts a resource type to its string representation.
 * @param res The resource type to convert.
 * @return A string representing the resource type, or NULL if the resource is invalid.
 * @note This function is used to convert resource types into human-readable strings for display purposes.
 */
bool res_from_string(const char *name, res_t *out);
/**
 * @brief Spawns resources on the game world if needed.
 * @param w Pointer to the world structure where resources will be spawned.
 * @param id The resource type to spawn.
 * @param need The number of resources needed to be spawned.
 * @note This function checks if the specified resource type needs to be spawned in the game world
 *       and spawns the required amount if necessary. It is used to manage resource availability in the game.
 * @note The function ensures that resources are available for players to collect and use during gameplay.
 */
void spawn_resources_if_needed(world_t *w, res_t id, int need);
/**
 * @brief Retrieves a tile from the game world at specified coordinates.
 * @param w Pointer to the world structure.
 * @param x The x-coordinate of the tile.
 * @param y The y-coordinate of the tile.
 * @return A pointer to the tile at the specified coordinates, or NULL if the coordinates are out of bounds.
 * @note This function allows access to specific tiles in the game world, enabling players and game logic to interact with resources on those tiles.
 */
static inline tile_t *world_get_tile(world_t *w, int x, int y)
{
    if (!w || !w->tiles || w->w <= 0 || w->h <= 0)
        return NULL;
    x = (x % w->w + w->w) % w->w;
    y = (y % w->h + w->h) % w->h;
    return &w->tiles[y * w->w + x];
}

#endif /* WORLD_H */
