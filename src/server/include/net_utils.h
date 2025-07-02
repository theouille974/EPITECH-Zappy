/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** net_utils
*/

#ifndef NET_UTILS_H
    #define NET_UTILS_H

    #include <stdbool.h>
    #include "net_poll.h"

/**
 * * @brief Sets a file descriptor to non-blocking mode.
 * * @param fd The file descriptor to set to non-blocking mode.
 * * @return True if the operation was successful, false otherwise.
 * * @note This function uses fcntl to change the file descriptor flags to non-blocking.
 * * @note If the fcntl call fails, it returns false.
 */
bool set_nonblock(int fd);
/**
 * * @brief Sets up a listening socket for incoming connections.
 * * @param net Pointer to the network structure containing the game state.
 * * @param port The port number to listen on.
 * * @return True if the socket was successfully set up, false otherwise.
 * * @note This function creates a socket, sets it to non-blocking mode, and binds it to the specified port.
 * * @note If any step fails (socket creation, binding, or listening), it returns false.
 */
bool setup_listen_socket(net_t *net, int port);

#endif /* NET_UTILS_H */
