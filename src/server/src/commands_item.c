/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** commands
*/

#include "item_commands.h"
#include "item_helpers.h"
#include "world.h"
#include "gui.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdint.h>

static uint64_t inv_now_ms(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000ULL + tv.tv_usec / 1000ULL;
}

static void exec_inventory(struct s_player *p)
{
    char buf[192];
    uint64_t period_ms = 126000ULL / (uint64_t)p->freq;
    uint64_t now = inv_now_ms();
    uint64_t first_slice_ms =
        (p->next_food > now) ? (p->next_food - now) : 0ULL;
    uint64_t ttl_ms = first_slice_ms + (uint64_t)p->inv[RES_FOOD] * period_ms;
    uint64_t ttl_sec = ttl_ms / 1000ULL;

    snprintf(buf, sizeof(buf),
        "[ food %u, linemate %u, deraumere %u, sibur %u, mendiane %u, "
        "phiras %u, thystame %u, life %llu ]\n",
        p->inv[RES_FOOD], p->inv[RES_LINEMATE], p->inv[RES_DERAUMERE],
        p->inv[RES_SIBUR], p->inv[RES_MENDIANE], p->inv[RES_PHIRAS],
        p->inv[RES_THYSTAME], (unsigned long long)ttl_sec);
    ih_reply(p->fd, buf);
    if (p->q_len > 0)
        --p->q_len;
}

static void exec_item_action(struct s_player *raw)
{
    item_ctx_t *ctx = (item_ctx_t *)raw;
    player_t *p = ctx->pl;
    tile_t *t = &p->world->tiles[p->y * p->world->w + p->x];
    bool ok = ctx->take
        ? ih_perform_take(p, ctx->id, t)
        : ih_perform_set(p, ctx->id, t);

    ih_reply(p->fd, ok ? "ok\n" : "ko\n");
    if (ok && p->net) {
        if (ctx->take)
            gui_broadcast_pgt(p->net, p, ctx->id);
        else
            gui_broadcast_pdr(p->net, p, ctx->id);
        gui_refresh_bct_pin(p->net, p);
    }
    if (p->q_len > 0)
        --p->q_len;
    free(ctx);
}

static bool schedule_inventory(player_t *pl, scheduler_t *sched, int freq)
{
    action_t act = {0};
    bool ok;

    act.exec_at = ih_now_ms() + (1000ULL) / (uint64_t)freq;
    act.fn = exec_inventory;
    act.pl = pl;
    ok = scheduler_push(sched, act);
    if (ok)
        pl->q_len += 1;
    return ok;
}

static bool schedule_item_action(player_t *pl,
    scheduler_t *sched,
    int freq,
    item_params_t params)
{
    item_ctx_t *ctx = malloc(sizeof(*ctx));
    action_t act = {0};
    bool ok2;

    if (!ctx)
        return false;
    ctx->pl = pl;
    ctx->id = params.id;
    ctx->take = (params.op == ITEM_OP_TAKE);
    act.exec_at = ih_now_ms() + (7 * 1000ULL) / (uint64_t)freq;
    act.fn = exec_item_action;
    act.pl = (player_t *)ctx;
    ok2 = scheduler_push(sched, act);
    if (ok2)
        pl->q_len += 1;
    return ok2;
}

static bool parse_item_cmd(const char *line, item_params_t *out)
{
    char w1[16] = {0};
    char w2[16] = {0};

    if (ih_str_eq(line, "Inventory")) {
        out->op = ITEM_OP_INVENTORY;
        return true;
    }
    if (sscanf(line, "%15s %15s", w1, w2) != 2)
        return false;
    if (ih_str_eq(w1, "Take"))
        out->op = ITEM_OP_TAKE;
    else if (ih_str_eq(w1, "Set"))
        out->op = ITEM_OP_SET;
    else
        return false;
    return ih_res_from_string(w2, &out->id);
}

bool try_push_item_cmd(const char *line,
    player_t *pl,
    scheduler_t *sched,
    int freq)
{
    item_params_t p;

    if (!parse_item_cmd(line, &p))
        return false;
    if (p.op == ITEM_OP_INVENTORY)
        return schedule_inventory(pl, sched, freq);
    return schedule_item_action(pl, sched, freq, p);
}
