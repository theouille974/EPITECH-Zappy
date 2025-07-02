/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** net_client
*/

#include "net_poll.h"
#include "player.h"
#include <stdbool.h>
#include <sys/types.h>

#ifndef NET_CLIENT_H
    #define NET_CLIENT_H

/**
 * @brief Drops a file descriptor from the network.
 * @param net Pointer to the network structure containing the game state.
 * @param idx The index of the file descriptor to be dropped.
 * @note This function removes the specified file descriptor from the network's poll file descriptors and cleans up the associated player structure.
 * @note It is typically called when a client disconnects or when a file descriptor is no longer needed.
 */
void drop_fd(net_t *net, int idx);
/**
 * @brief Handles a client connection.
 * @param net Pointer to the network structure containing the game state.
 * @param idx The index of the file descriptor for the client.
 * @note This function processes incoming data from the client, manages authentication, and handles client commands.
 * @note It is typically called in a loop to continuously handle client interactions.
 */
void handle_client(net_t *net, int idx);
/**
 * @brief Handles a client disconnection.
 * @param net Pointer to the network structure containing the game state.
 * @param idx The index of the file descriptor for the client.
 * @note This function cleans up resources associated with a disconnected client and updates the network state accordingly.
 */
void exec_periodic_refill(struct s_player *raw);

/**
 * @brief Assigns a player to a team.
 * @param net Pointer to the network structure containing the game state.
 * @param idx The index of the player in the network.
 * @param pl Pointer to the player structure to be assigned.
 * @param team_name The name of the team to assign the player to.
 * @return True if the assignment was successful, false otherwise.
 * @note This function checks if the team exists, assigns the player to the team, and updates the player's team index.
 * @note It also handles the case where the player is already assigned to a team.
 * @note If the team does not exist, it returns false without modifying the player's state.
 */
bool assign_team(net_t *net, int idx, player_t *pl, const char *team_name);

/**
 * @brief Structure representing the components of the server.
 * @param world Pointer to the world structure containing the game state.
 * @param sched Pointer to the scheduler managing game actions.
 * @param teams Array of pointers to team structures.
 * @param net Pointer to the network structure managing client connections.
 * @note This structure encapsulates all the components required for the server to function, including the game world, scheduling, teams, and network management.
 */
typedef struct server_components_s {
    world_t *world;
    scheduler_t *sched;
    team_t **teams;
    net_t *net;
} server_components_t;

#endif /* NET_CLIENT_H */
