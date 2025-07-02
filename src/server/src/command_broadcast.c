/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** command_broadcast.c
*/

#include "command_broadcast.h"
#include "net_poll.h"
#include "gui.h"
#include "command_broadcast_utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/time.h>

static uint64_t now_ms(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000ULL + tv.tv_usec / 1000ULL;
}

/* Compute shortest wrapped delta between two coords on an axis */
static int wrap_delta(int a, int b, int max)
{
    int d = a - b;

    if (d > max / 2)
        d -= max;
    if (d < (-max) / 2)
        d += max;
    return d;
}

/* Return base direction code relative to NORTH-facing receiver (1..8) */
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

/* Rotate base direction according to receiver orientation */
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

static bool is_valid_receiver(const player_t *rcv)
{
    return rcv && rcv->authed && !IS_GUI(rcv);
}

static void broadcast_to_players(const broadcast_ctx_t *ctx)
{
    player_t *em = ctx->pl;
    net_t *net = em->net;
    char line[256];
    player_t *rcv;
    int dir;

    if (!net)
        return;
    for (int i = 1; i < net->nfds; ++i) {
        rcv = (player_t *)net->players[i];
        if (!is_valid_receiver(rcv))
            continue;
        dir = compute_direction(em, rcv);
        snprintf(line, sizeof(line), "message %d, %s\n", dir, ctx->msg);
        write(rcv->fd, line, strlen(line));
    }
    gui_broadcast_pbc(net, em, ctx->msg);
}

static void exec_broadcast(struct s_player *raw)
{
    broadcast_ctx_t *ctx = (broadcast_ctx_t *)raw;
    player_t *em = ctx->pl;

    broadcast_to_players(ctx);
    if (em->q_len > 0)
        --em->q_len;
    free(ctx->msg);
    free(ctx);
}

static bool enqueue_broadcast_action(broadcast_ctx_t *ctx,
    scheduler_t *sched, int freq)
{
    action_t act = {0};

    act.exec_at = now_ms() + (7 * 1000ULL) / (uint64_t)freq;
    act.fn = exec_broadcast;
    act.pl = (player_t *)ctx;
    return scheduler_push(sched, act);
}

bool try_push_broadcast_cmd(const char *line,
    player_t *pl,
    scheduler_t *sched,
    int freq)
{
    const char *prefix = "Broadcast";
    char *text = extract_broadcast_text(line, prefix);
    broadcast_ctx_t *ctx;

    if (!text)
        return false;
    ctx = create_broadcast_ctx(pl, text);
    if (!ctx) {
        free(text);
        return false;
    }
    if (!enqueue_broadcast_action(ctx, sched, freq)) {
        free(ctx->msg);
        free(ctx);
        return false;
    }
    pl->q_len += 1;
    return true;
}
