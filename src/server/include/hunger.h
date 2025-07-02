/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** hunger
*/

#include <stdint.h>
#include <unistd.h>
#include "net_poll.h"

#ifndef HUNGER_H
    #define HUNGER_H

/**
 * @brief Updates the hunger level of a player.
 * @param net Pointer to the network structure containing the game state.
 * @param now The current time in milliseconds.
 */
void hunger_check(net_t *net, uint64_t now);

#endif /* HUNGER_H */
