/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** gui_broadcast_misc.c
*/

#include "gui.h"
#include <stdio.h>
#include <unistd.h>

void gui_broadcast_pbc(net_t *net, const player_t *pl, const char *text)
{
    char line[GUI_BUF_SZ];
    int n;

    if (!text)
        text = "";
    n = snprintf(line, sizeof(line), "pbc #%d %s\n", pl->fd, text);
    if (n < 0)
        return;
    broadcast(net, line, (size_t)n);
}

void gui_broadcast_seg(net_t *net, const char *team_name)
{
    char line[GUI_BUF_SZ];
    int n;

    if (!team_name)
        team_name = "";
    n = snprintf(line, sizeof(line), "seg %s\n", team_name);
    if (n < 0)
        return;
    broadcast(net, line, (size_t)n);
}

void gui_broadcast_smg(net_t *net, const char *msg)
{
    char line[GUI_BUF_SZ];
    int n;

    if (!msg)
        msg = "";
    n = snprintf(line, sizeof(line), "smg %s\n", msg);
    if (n < 0)
        return;
    broadcast(net, line, (size_t)n);
}
