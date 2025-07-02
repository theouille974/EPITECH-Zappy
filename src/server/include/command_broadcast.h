/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** command_broadcast.h
*/

#ifndef COMMAND_BROADCAST_H
    #define COMMAND_BROADCAST_H

    #define _POSIX_C_SOURCE 200809L

    #include "scheduler.h"
    #include "player.h"

/**
 * @brief Context structure for broadcasting messages.
 * @note This structure holds the player who is broadcasting and the message to be sent.
 * @note It is used to encapsulate the broadcast command and its associated player.
 */
typedef struct broadcast_ctx_s {
    player_t *pl;
    char *msg;
} broadcast_ctx_t;

/**
 * @brief Attempts to push a broadcast command to the scheduler.
 * @param line The input line containing the broadcast command.
 * @param pl The player who is broadcasting.
 * @param sched The scheduler to which the command will be added.
 * @param freq The frequency at which the command should be executed.
 * @return True if the command was successfully pushed, false otherwise.
 * @note This function checks if the broadcast command is valid and adds it to the scheduler.
 */
bool try_push_broadcast_cmd(const char *line, player_t *pl,
    scheduler_t *sched, int freq);

#endif /* COMMAND_BROADCAST_H */
