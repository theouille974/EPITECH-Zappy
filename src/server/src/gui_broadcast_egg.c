/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** gui_broadcast_egg.c
*/

#include "gui.h"
#include <stdio.h>
#include <unistd.h>

void gui_broadcast_enw(net_t *net, const gui_enw_t *info)
{
    char line[GUI_BUF_SZ];
    int n;

    if (!info)
        return;
    n = snprintf(line, sizeof(line), "enw #%d #%d %d %d\n",
        info->egg_id, info->player_id, info->x, info->y);
    if (n < 0)
        return;
    broadcast(net, line, (size_t)n);
}

void gui_broadcast_ebo(net_t *net, int egg_id)
{
    char line[GUI_BUF_SZ];
    int n = snprintf(line, sizeof(line), "ebo #%d\n", egg_id);

    if (n < 0)
        return;
    broadcast(net, line, (size_t)n);
}

void gui_broadcast_edi(net_t *net, int egg_id)
{
    char line[GUI_BUF_SZ];
    int n = snprintf(line, sizeof(line), "edi #%d\n", egg_id);

    if (n < 0)
        return;
    broadcast(net, line, (size_t)n);
}
