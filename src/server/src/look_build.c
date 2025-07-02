/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** Look build
*/

#include "command_look.h"
#include "player.h"
#include "world.h"
#include "net_poll.h"
#include <string.h>

static const char *RES_NAMES[RES_MAX] = {
    "food", "linemate", "deraumere", "sibur",
    "mendiane", "phiras", "thystame"
};

int wrap_coord(int v, int max)
{
    v %= max;
    if (v < 0)
        v += max;
    return v;
}

static size_t add_word(buf_ctx_t *b, const char *word)
{
    size_t used = 0;

    if (!b || !b->rem)
        return 0;
    if (*b->need_sp && b->rem) {
        *(b->dst) = ' ';
        b->dst += 1;
        used += 1;
        b->rem -= 1;
    }
    while (*word && b->rem) {
        *(b->dst) = *word;
        b->dst += 1;
        used += 1;
        word += 1;
        b->rem -= 1;
    }
    *b->need_sp = true;
    return used;
}

static size_t add_players(const net_t *net, int x, int y,
    buf_ctx_t *b)
{
    size_t used = 0;
    int idx;
    player_t *p;

    for (idx = 1; idx < net->nfds && used < b->rem; ++idx) {
        p = (player_t *)net->players[idx];
        if (!p || !p->authed || p->team_idx < 0)
            continue;
        if (p->x == x && p->y == y)
            used += add_word(b, "player");
    }
    return used;
}

static size_t add_resources(const tile_t *t, buf_ctx_t *b)
{
    size_t used = 0;
    res_t r;
    uint16_t c;

    for (r = 0; r < RES_MAX && used < b->rem; ++r)
        for (c = 0; c < t->res[r] && used < b->rem; ++c)
            used += add_word(b, RES_NAMES[r]);
    return used;
}

size_t build_tile(const net_t *net, int x, int y, buf_ctx_t *b)
{
    bool need = false;
    size_t used;

    b->need_sp = &need;
    used = add_players(net, x, y, b);
    used += add_resources(&net->world->tiles[y * net->world->w + x], b);
    *b->need_sp = true;
    return used;
}
