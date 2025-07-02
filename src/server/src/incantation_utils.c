/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** incantation_utils.c
*/

#include "incantation_utils.h"
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "gui.h"

uint64_t inc_now_ms(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000ULL + tv.tv_usec / 1000ULL;
}

int inc_count_players(const net_t *net, int x, int y, int lvl)
{
    int cnt = 0;
    const player_t *p;

    for (int i = 1; i < net->nfds; ++i) {
        p = (const player_t *)net->players[i];
        if (!p || !p->authed || p->team_idx < 0)
            continue;
        if (p->x == x && p->y == y && p->level == lvl)
            ++cnt;
    }
    return cnt;
}

bool inc_resources_sufficient(const tile_t *t, const int stones[RES_MAX])
{
    for (res_t id = 0; id < RES_MAX; ++id) {
        if (t->res[id] < (uint16_t)stones[id])
            return false;
    }
    return true;
}

void inc_consume_resources(tile_t *t, const int stones[RES_MAX])
{
    for (res_t id = 0; id < RES_MAX; ++id)
        t->res[id] -= (uint16_t)stones[id];
}

static void inc_send_level_msg(player_t *pl)
{
    char buf[32];
    int n = snprintf(buf, sizeof(buf),
        "Current level: %d\n", pl->level);

    if (n > 0)
        write(pl->fd, buf, (size_t)n);
    if (pl->q_len > 0)
        --pl->q_len;
}

void inc_level_up_players(net_t *net, int x, int y, int lvl)
{
    player_t *pl;

    for (int i = 1; i < net->nfds; ++i) {
        pl = (player_t *)net->players[i];
        if (!pl || pl->level != lvl ||
            pl->x != x || pl->y != y)
            continue;
        pl->level += 1;
        inc_send_level_msg(pl);
        gui_broadcast_plv(net, pl);
    }
}
