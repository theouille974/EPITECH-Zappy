/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** net_poll
*/

#include <poll.h>
#include <stdbool.h>
#include "scheduler.h"
#include "world.h"
#include "team.h"
#include "egg.h"

#ifndef NET_POLL_H
    #define NET_POLL_H
    #define NET_MAX_FDS 128

/**
 * @brief Structure representing the network state.
 * @param listen_fd The file descriptor for the listening socket.
 * @param pfds Array of poll file descriptors for managing client connections.
 * @param players Array of pointers to player structures associated with each file descriptor.
 * @param nfds The number of file descriptors currently being monitored.
 * @param sched Pointer to the scheduler managing game actions.
 * @param freq The frequency of the scheduler.
 * @param world Pointer to the world structure containing the game state.
 * @param teams Array of team structures representing the teams in the game.
 * @param team_cnt The number of teams in the game.
 * @param eggs Array of egg structures representing the eggs in the game.
 * @param egg_count The current count of eggs in the game.
 * @param next_egg_id The ID to be assigned to the next egg created.
 * @note This structure encapsulates the network state, including client connections, game scheduling, and the game world.
 * @note It is used to manage player connections, team assignments, and egg management.
 */
typedef struct s_net {
    int listen_fd;
    struct pollfd pfds[NET_MAX_FDS + 1];
    void *players[NET_MAX_FDS + 1];
    int nfds;
    scheduler_t *sched;
    int freq;
    struct s_world *world;
    team_t *teams;
    int team_cnt;
    egg_t eggs[EGG_MAX];
    int egg_count;
    int next_egg_id;
} net_t;

/**
 * @brief Structure representing the parameters for network initialization.
 * @param port The port number for the server to listen on.
 * @param world Pointer to the world structure containing the game state.
 * @param teams Array of team structures representing the teams in the game.
 * @param team_cnt The number of teams in the game.
 * @param sched Pointer to the scheduler managing game actions.
 * @param freq The frequency of the scheduler.
 * @note This structure is used to pass parameters during network initialization, allowing for flexible configuration of the server.
 */
typedef struct s_net_params {
    int port;
    struct s_world *world;
    team_t *teams;
    int team_cnt;
    scheduler_t *sched;
    int freq;
} net_params_t;

/**
 * @brief Initializes the network state.
 * @param net Pointer to the net_t structure to be initialized.
 * @param p Pointer to the net_params_t structure containing initialization parameters.
 * @return True if initialization was successful, false otherwise.
 * @note This function sets up the network state, including the listening socket, poll file descriptors, and player structures.
 */
bool net_init(net_t *net, const net_params_t *p);
/**
 * @brief Polls the network for events.
 * @param net Pointer to the net_t structure representing the network state.
 * @param timeout_ms The timeout in milliseconds for the poll operation.
 * @return True if there are events to process, false if the timeout expired without events.
 * @note This function checks for incoming data on client connections and processes any events that occur.
 */
void net_poll_once(net_t *net, int timeout_ms);
/**
 * @brief Handles network events.
 * @param net Pointer to the net_t structure representing the network state.
 * @return True if events were processed, false if there are no more events to process.
 * @note This function processes incoming data from clients, handles client connections and disconnections, and manages game actions.
 */
void net_shutdown(net_t *net);

#endif /* NET_POLL_H */
