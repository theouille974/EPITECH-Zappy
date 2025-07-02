/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** gui_commands.c
*/

#include "gui.h"
#include "world.h"
#include "player.h"
#include "team.h"
#include <unistd.h>
#include <stdio.h>

void gui_broadcast_full_map(net_t *net)
{
    for (int y = 0; y < net->world->h; ++y)
        for (int x = 0; x < net->world->w; ++x)
            gui_broadcast_tile(net, x, y);
}

void gui_broadcast_pin(net_t *net, const player_t *pl)
{
    char line[GUI_BUF_SZ];
    player_t *cli;
    int n = snprintf(line, sizeof(line),
        "pin #%d %d %d %u %u %u %u %u %u %u\n",
        pl->fd, pl->x, pl->y,
        pl->inv[RES_FOOD], pl->inv[RES_LINEMATE],
        pl->inv[RES_DERAUMERE], pl->inv[RES_SIBUR],
        pl->inv[RES_MENDIANE], pl->inv[RES_PHIRAS],
        pl->inv[RES_THYSTAME]);

    if (n < 0)
        return;
    gui_broadcast_tile(net, pl->x, pl->y);
    for (int i = 1; i < net->nfds; ++i) {
        cli = (player_t *)net->players[i];
        if (IS_GUI(cli))
            write(net->pfds[i].fd, line, (size_t)n);
    }
}
