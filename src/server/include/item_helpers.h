/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** net_client
*/

#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include "world.h"
#include "player.h"

#ifndef ITEM_HELPERS_H
    #define ITEM_HELPERS_H

/**
 * @brief Checks if two strings are equal.
 * @param a The first string to compare.
 * @param b The second string to compare.
 * @return True if the strings are equal, false otherwise.
 */
static inline bool ih_str_eq(const char *a, const char *b)
{
    return !strcmp(a, b);
}

/**
 * @brief Gets the current time in milliseconds.
 * @return The current time in milliseconds since the epoch.
 */
uint64_t ih_now_ms(void);
/**
 * @brief Sends a reply message to a client.
 * @param fd The file descriptor of the client socket.
 * @param msg The message to be sent.
 */
void ih_reply(int fd, const char *msg);
/**
 * @brief Converts a resource name string to its corresponding resource ID.
 * @param word The resource name string.
 * @param out Pointer to the variable where the resource ID will be stored.
 * @return True if the conversion was successful, false otherwise.
 */
bool ih_res_from_string(const char *word, res_t *out);
/**
 * @brief Performs a take operation for a resource on a tile.
 * @param p Pointer to the player performing the operation.
 * @param id The resource ID to take.
 * @param t Pointer to the tile where the resource is located.
 * @return True if the operation was successful, false otherwise.
 */
bool ih_perform_take(player_t *p, res_t id, tile_t *t);
/**
 * @brief Performs a set operation for a resource on a tile.
 * @param p Pointer to the player performing the operation.
 * @param id The resource ID to set.
 * @param t Pointer to the tile where the resource will be set.
 * @return True if the operation was successful, false otherwise.
 */
bool ih_perform_set(player_t *p, res_t id, tile_t *t);

#endif
