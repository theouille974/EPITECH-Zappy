/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** player
*/

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "world.h"

#ifndef PLAYER_H
    #define PLAYER_H
    #define PLAYER_BUF_SZ 1024
    #define PLAYER_QUEUE_MAX 10

struct s_scheduler;
struct s_world;
struct s_net;

/**
 * @brief Structure representing a player in the game.
 * @param fd The file descriptor for the player's socket connection.
 * @param buf The buffer for incoming data from the player.
 * @param len The length of the data in the buffer.
 * @param queue The queue of commands for the player.
 * @param q_len The length of the command queue.
 * @param x The X coordinate of the player's position on the map.
 * @param y The Y coordinate of the player's position on the map.
 * @param dir The direction the player is facing (0-3).
 * @param level The player's current level.
 * @param world Pointer to the world structure containing the game state.
 * @param net Pointer to the network structure managing client connections.
 * @param authed Indicates whether the player has been authenticated.
 * @param team_idx The index of the team the player belongs to.
 * @param inv The player's inventory, represented as an array of resource counts.
 * @param freq The frequency of the player's actions in the game.
 * @param next_food The timestamp for the next food consumption check.
 * @note This structure encapsulates all the necessary information about a player, including their connection details, position, direction, level, team affiliation, inventory, and game state management.
 * @note It is used to manage player interactions, movements, and actions within the game world.
 */
typedef struct s_player {
    int fd;
    char buf[PLAYER_BUF_SZ];
    size_t len;
    char *queue[PLAYER_QUEUE_MAX];
    int q_len;
    int x;
    int y;
    int dir;
    int level;
    struct s_world *world;
    struct s_net *net;
    bool authed;
    int team_idx;
    uint16_t inv[RES_MAX];
    int freq;
    uint64_t next_food;
} player_t;

/**
 * @brief Creates a new player instance.
 * @param fd The file descriptor for the player's socket connection.
 * @param world Pointer to the world structure containing the game state.
 * @param freq The frequency of the player's actions in the game.
 * @param net Pointer to the network structure managing client connections.
 * @return A pointer to the newly created player instance, or NULL on failure.
 * @note This function initializes a player with default values and prepares them for interaction within the game world.
 */
player_t *player_create(int fd, struct s_world *world,
    int freq, struct s_net *net);
/**
 * @brief Destroys a player instance and frees associated resources.
 * @param p Pointer to the player instance to be destroyed.
 * @note This function cleans up the player's resources, including their command queue and network connection.
 */
void player_destroy(player_t *p);

/**
 * @brief Feeds data to a player, processing it and executing commands.
 * @param p Pointer to the player instance.
 * @param data Pointer to the data to be fed to the player.
 * @param n The size of the data to be fed.
 * @param sched Pointer to the scheduler managing game events.
 * @return True if the data was successfully processed, false otherwise.
 */
bool player_feed(player_t *p, const char *data, size_t n,
    struct s_scheduler *sched);

#endif /* PLAYER_H */
