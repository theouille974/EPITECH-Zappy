/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** hunger
*/

#include "hunger.h"
#include "player.h"
#include "net_client.h"
#include "gui.h"

static void starve_player(net_t *net, int idx)
{
    player_t *pl = (player_t *)net->players[idx];

    if (!pl)
        return;
    write(pl->fd, "dead\n", 5);
    gui_broadcast_pdi(net, pl);
    drop_fd(net, idx);
}

static void update_player_hunger(net_t *net, int idx, uint64_t now,
    uint64_t period)
{
    player_t *pl = (player_t *)net->players[idx];
    uint64_t ticks;

    if (!pl || !pl->authed || pl->team_idx < 0)
        return;
    if (now < pl->next_food)
        return;
    ticks = (now - pl->next_food) / period + 1;
    if (pl->inv[RES_FOOD] >= ticks) {
        pl->inv[RES_FOOD] -= (uint16_t)ticks;
        pl->next_food += period * ticks;
        gui_broadcast_pin(net, pl);
    } else {
        starve_player(net, idx);
    }
}

void hunger_check(net_t *net, uint64_t now)
{
    uint64_t period = 126000ULL / (uint64_t)net->freq;
    int i;

    for (i = 1; i < net->nfds; ++i)
        update_player_hunger(net, i, now, period);
}
