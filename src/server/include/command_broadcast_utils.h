/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** command_broadcast_utils.h
*/

#ifndef COMMAND_BROADCAST_UTILS_H
    #define COMMAND_BROADCAST_UTILS_H

    #include "command_broadcast.h"
    #include "player.h"

/**
 * @brief Extracts the broadcast text from a line of input.
 * @param line The input line containing the broadcast command.
 * @param prefix The prefix that identifies the broadcast command.
 * @return A dynamically allocated string containing the broadcast text, or NULL if the prefix is not found
 * @note or if the line is empty after the prefix.
 * @note The caller is responsible for freeing the returned string.
 */
char *extract_broadcast_text(const char *line, const char *prefix);

/**
 * @brief Creates a broadcast context for a player with the given text.
 * @param pl The player who is broadcasting.
 * @param text The broadcast message text.
 * @return A pointer to a dynamically allocated broadcast_ctx_t structure, or NULL on failure.
 * @note The caller is responsible for freeing the returned context.
 */
broadcast_ctx_t *create_broadcast_ctx(player_t *pl, char *text);

#endif /* COMMAND_BROADCAST_UTILS_H */
