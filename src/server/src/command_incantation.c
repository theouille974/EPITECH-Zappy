/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** commands_incantation
*/

#include "incantation.h"
#include "world.h"
#include "net_poll.h"
#include "gui.h"
#include "incantation_utils.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

const req_t REQS[8] = {
    {0, {0}},
    {1, {0, 1, 0, 0, 0, 0, 0}},
    {2, {0, 1, 1, 1, 0, 0, 0}},
    {2, {0, 2, 0, 1, 0, 2, 0}},
    {4, {0, 1, 1, 2, 0, 1, 0}},
    {4, {0, 1, 2, 1, 3, 0, 0}},
    {6, {0, 1, 2, 3, 0, 1, 0}},
    {6, {0, 2, 2, 2, 2, 2, 1}},
};

static bool reqs_met(player_t *pl,
    const tile_t *tile,
    const req_t *req)
{
    int cnt = inc_count_players(pl->net,
        pl->x, pl->y, pl->level);

    if (cnt < req->players)
        return false;
    return inc_resources_sufficient(tile, req->stones);
}

static void handle_failure(player_t *init)
{
    const char *ko = "ko\n";

    write(init->fd, ko, strlen(ko));
    if (init->q_len > 0)
        --init->q_len;
}

static void handle_success(net_t *net, inc_ctx_t *ctx, tile_t *tile)
{
    inc_consume_resources(tile, REQS[ctx->level].stones);
    inc_level_up_players(net, ctx->x, ctx->y, ctx->level);
}

static tile_t *tile_at(world_t *w, int x, int y)
{
    return world_get_tile(w, x, y);
}

static bool validate_incantation_context(inc_ctx_t *ctx,
    player_t **init, net_t **net)
{
    *init = ctx->initiator;
    *net = *init ? (*init)->net : NULL;
    if (!*init || !*net)
        return false;
    return true;
}

static int check_incantation_requirements(player_t *init,
    tile_t *tile, int level)
{
    int success_start = reqs_met(init, tile, &REQS[level]);
    int success_end = reqs_met(init, tile, &REQS[level]);

    return (success_start && success_end) ? 1 : 0;
}

static void handle_incantation_result(inc_result_ctx_t *result_ctx)
{
    if (result_ctx->success)
        handle_success(result_ctx->net, result_ctx->ctx, result_ctx->tile);
    else
        handle_failure(result_ctx->init);
    gui_broadcast_pie(result_ctx->net,
        result_ctx->ctx->x, result_ctx->ctx->y, result_ctx->success);
    gui_broadcast_tile(result_ctx->net,
        result_ctx->ctx->x, result_ctx->ctx->y);
}

static void exec_incantation(struct s_player *raw)
{
    inc_ctx_t *ctx = (inc_ctx_t *)raw;
    player_t *init;
    net_t *net;
    tile_t *tile;
    int success;
    inc_result_ctx_t result_ctx;

    if (!validate_incantation_context(ctx, &init, &net)) {
        free(ctx);
        return;
    }
    tile = tile_at(net->world, ctx->x, ctx->y);
    success = check_incantation_requirements(init, tile, ctx->level);
    result_ctx.net = net;
    result_ctx.ctx = ctx;
    result_ctx.tile = tile;
    result_ctx.init = init;
    result_ctx.success = success;
    handle_incantation_result(&result_ctx);
    free(ctx);
}

bool schedule_incantation(player_t *pl,
    scheduler_t *sched, int freq)
{
    inc_ctx_t *ctx = malloc(sizeof(*ctx));
    action_t act = {0};

    if (!ctx)
        return false;
    *ctx = (inc_ctx_t){ .initiator = pl,
        .x = pl->x, .y = pl->y, .level = pl->level };
    act.exec_at = inc_now_ms() +
        (INCANTATION_DELAY * 1000ULL) / (uint64_t)freq;
    act.fn = exec_incantation;
    act.pl = (player_t *)ctx;
    if (!scheduler_push(sched, act)) {
        free(ctx);
        return false;
    }
    pl->q_len += 1;
    return true;
}
