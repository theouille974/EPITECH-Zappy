/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** gui_pgt_pdr
*/

#include "gui.h"
#include "world.h"
#include <stdio.h>
#include <unistd.h>

static void broadcast_to_guis(net_t *net, const char *buf, size_t len)
{
    player_t *cli;

    for (int i = 1; i < net->nfds; ++i) {
        cli = (player_t *)net->players[i];
        if (IS_GUI(cli))
            write(net->pfds[i].fd, buf, len);
    }
}

void gui_broadcast_pgt(net_t *net, const player_t *pl, res_t res)
{
    char buf[GUI_BUF_SZ];
    int n = snprintf(buf, sizeof(buf), "pgt #%d %d\n", pl->fd, res);

    if (n < 0)
        return;
    broadcast_to_guis(net, buf, (size_t)n);
}

void gui_broadcast_pdr(net_t *net, const player_t *pl, res_t res)
{
    char buf[GUI_BUF_SZ];
    int n = snprintf(buf, sizeof(buf), "pdr #%d %d\n", pl->fd, res);

    if (n < 0)
        return;
    broadcast_to_guis(net, buf, (size_t)n);
}

void gui_refresh_bct_pin(net_t *net, const player_t *pl)
{
    if (!net || !pl)
        return;
    gui_broadcast_tile(net, pl->x, pl->y);
    gui_broadcast_pin(net, pl);
}
