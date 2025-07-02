/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** command_eject.c - implementation of the "Eject" command (pushes all other
** players present on the same tile one square in the issuer's facing
** direction).
*/

#include "command_handlers.h"
#include "player.h"
#include "world.h"
#include "gui.h"
#include "egg.h"
#include "net_poll.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

static const int DX[4] = {0, 1, 0, -1};
static const int DY[4] = {-1, 0, 1, 0};

static int wrap_delta(int a, int b, int max)
{
    int d = a - b;

    if (d > max / 2)
        d -= max;
    if (d < (-max) / 2)
        d += max;
    return d;
}

static int base_dir_from_delta(int dx, int dy)
{
    if (dx == 0 && dy == 0)
        return 0;
    if (dx == 0)
        return (dy < 0) ? 1 : 5;
    if (dy == 0)
        return (dx > 0) ? 3 : 7;
    if (dx > 0 && dy < 0)
        return 2;
    if (dx > 0 && dy > 0)
        return 4;
    if (dx < 0 && dy > 0)
        return 6;
    return 8;
}

static int apply_orientation(int base, int ori)
{
    int shift;
    int k;

    if (base == 0)
        return 0;
    shift = (ori * 2) % 8;
    k = base - 1;
    k = (k - shift + 8) % 8;
    return k + 1;
}

static int compute_direction(const player_t *emitter, const player_t *receiver)
{
    int dx = wrap_delta(emitter->x, receiver->x, receiver->world->w);
    int dy = wrap_delta(emitter->y, receiver->y, receiver->world->h);
    int base = base_dir_from_delta(dx, dy);

    return apply_orientation(base, receiver->dir);
}

static bool try_eject_player(player_t *issuer, player_t *target)
{
    net_t *net = issuer->net;
    world_t *world = issuer->world;
    char buf[32];
    int dir_code;
    int n;

    if (!target || target == issuer || !target->authed || target->team_idx < 0)
        return false;
    if (target->x != issuer->x || target->y != issuer->y)
        return false;
    target->x = (target->x + DX[issuer->dir] + world->w) % world->w;
    target->y = (target->y + DY[issuer->dir] + world->h) % world->h;
    dir_code = compute_direction(issuer, target);
    n = snprintf(buf, sizeof(buf), "eject: %d\n", dir_code);
    if (n > 0)
        write(target->fd, buf, (size_t)n);
    gui_broadcast_ppo(net, target);
    return true;
}

static bool eject_others(player_t *p)
{
    net_t *net = p->net;
    bool any = false;

    for (int idx = 1; idx < net->nfds; ++idx)
        any |= try_eject_player(p, (player_t *)net->players[idx]);
    return any;
}

static void destroy_eggs_on_tile(net_t *net, int x, int y)
{
    int egg_id;

    if (!net)
        return;
    for (int i = 0; i < net->egg_count; ++i) {
        if (net->eggs[i].x == x && net->eggs[i].y == y) {
            egg_id = net->eggs[i].id;
            gui_broadcast_ebo(net, egg_id);
            net->eggs[i] = net->eggs[net->egg_count - 1];
            net->egg_count -= 1;
            --i;
        }
    }
}

void cmd_eject(struct s_player *p)
{
    const char *OK = "ok\n";
    const char *KO = "ko\n";
    net_t *net;
    bool pushed;

    if (!p || !p->net || !p->world)
        return;
    net = p->net;
    pushed = eject_others(p);
    write(p->fd, pushed ? OK : KO, pushed ? strlen(OK) : strlen(KO));
    if (p->q_len > 0)
        --p->q_len;
    gui_broadcast_tile(net, p->x, p->y);
    destroy_eggs_on_tile(net, p->x, p->y);
}
