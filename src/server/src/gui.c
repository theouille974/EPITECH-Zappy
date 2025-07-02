/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** gui.c
*/

#include "gui.h"
#include "world.h"
#include "team.h"
#include "player.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static void send_bct_fd(int fd, int x, int y, const tile_t *t)
{
    sock_printf(fd,
        "bct %d %d %u %u %u %u %u %u %u\n",
        x, y,
        t->res[RES_FOOD], t->res[RES_LINEMATE],
        t->res[RES_DERAUMERE], t->res[RES_SIBUR],
        t->res[RES_MENDIANE], t->res[RES_PHIRAS],
        t->res[RES_THYSTAME]);
}

static void send_full_map_fd(int fd, const world_t *w)
{
    for (int y = 0; y < w->h; ++y)
        for (int x = 0; x < w->w; ++x)
            send_bct_fd(fd, x, y, &w->tiles[y * w->w + x]);
}

static void send_all_players_fd(int fd, const net_t *net)
{
    int o;
    const player_t *pl;

    for (int i = 1; i < net->nfds; ++i) {
        pl = (const player_t *)net->players[i];
        if (!pl || !pl->authed || pl->team_idx < 0)
            continue;
        o = pl->dir + 1;
        sock_printf(fd,
            "pnw #%d %d %d %d %d %s\n",
            pl->fd, pl->x, pl->y, o, pl->level,
            net->teams[pl->team_idx].name);
    }
}

void gui_send_initial(net_t *net, int fd)
{
    sock_printf(fd, "msz %d %d\n", net->world->w, net->world->h);
    send_full_map_fd(fd, net->world);
    for (int i = 0; i < net->team_cnt; ++i)
        sock_printf(fd, "tna %s\n", net->teams[i].name);
    send_all_players_fd(fd, net);
    sock_printf(fd, "sgt %d\n", net->freq);
}

void gui_broadcast_tile(net_t *net, int x, int y)
{
    char line[GUI_BUF_SZ];
    size_t n;
    tile_t *t = &net->world->tiles[y * net->world->w + x];

    n = (size_t)snprintf(line, sizeof(line),
        "bct %d %d %u %u %u %u %u %u %u\n",
        x, y,
        t->res[RES_FOOD], t->res[RES_LINEMATE],
        t->res[RES_DERAUMERE], t->res[RES_SIBUR],
        t->res[RES_MENDIANE], t->res[RES_PHIRAS],
        t->res[RES_THYSTAME]);
    broadcast(net, line, n);
}

void gui_broadcast_pnw(net_t *net, const player_t *pl)
{
    char line[GUI_BUF_SZ];
    int o = pl->dir + 1;
    int n = snprintf(line, sizeof(line),
        "pnw #%d %d %d %d %d %s\n",
        pl->fd, pl->x, pl->y, o, pl->level,
        net->teams[pl->team_idx].name);

    if (n < 0)
        return;
    broadcast(net, line, (size_t)n);
}

void gui_broadcast_ppo(net_t *net, const player_t *pl)
{
    char line[GUI_BUF_SZ];
    int o = pl->dir + 1;
    int n = snprintf(line, sizeof(line),
        "ppo #%d %d %d %d\n",
        pl->fd, pl->x, pl->y, o);

    if (n < 0)
        return;
    broadcast(net, line, (size_t)n);
}

void gui_broadcast_pdi(net_t *net, const player_t *pl)
{
    char line[GUI_BUF_SZ];
    int n = snprintf(line, sizeof(line), "pdi #%d\n", pl->fd);

    if (n < 0)
        return;
    broadcast(net, line, (size_t)n);
}
