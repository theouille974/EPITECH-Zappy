/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** gui_broadcast_player.c
*/

#include "gui.h"
#include <stdio.h>
#include <unistd.h>

void gui_broadcast_pex(net_t *net, const player_t *pl)
{
    char line[GUI_BUF_SZ];
    int n = snprintf(line, sizeof(line), "pex #%d\n", pl->fd);

    if (n < 0)
        return;
    broadcast(net, line, (size_t)n);
}

void gui_broadcast_pfk(net_t *net, const player_t *pl)
{
    char line[GUI_BUF_SZ];
    int n = snprintf(line, sizeof(line), "pfk #%d\n", pl->fd);

    if (n < 0)
        return;
    broadcast(net, line, (size_t)n);
}

void gui_broadcast_plv(net_t *net, const player_t *pl)
{
    char line[GUI_BUF_SZ];
    int n = snprintf(line, sizeof(line), "plv #%d %d\n", pl->fd, pl->level);

    if (n < 0)
        return;
    broadcast(net, line, (size_t)n);
}
