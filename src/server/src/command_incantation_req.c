/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** command_incantation_req.c
*/

#include "incantation.h"
#include "world.h"
#include "net_poll.h"
#include "gui.h"
#include "incantation_utils.h"
#include <string.h>
#include <unistd.h>

static bool incantation_allowed(player_t *pl,
    const req_t *req, const tile_t *tile)
{
    if (inc_count_players(pl->net, pl->x, pl->y, pl->level) < req->players)
        return false;
    return inc_resources_sufficient(tile, req->stones);
}

static void send_incantation_start(player_t *pl)
{
    gui_pic_t pic = { .x = pl->x, .y = pl->y, .level = pl->level };
    int ids[64];
    size_t cnt = 0;
    player_t *o;

    for (int i = 1; i < pl->net->nfds && cnt < 64; ++i) {
        o = (player_t *)pl->net->players[i];
        if (o && o->authed && o->team_idx >= 0 &&
            o->x == pl->x && o->y == pl->y && o->level == pl->level) {
            ids[cnt] = o->fd;
            cnt++;
        }
    }
    pic.ids = ids;
    pic.count = cnt;
    gui_broadcast_pic(pl->net, &pic);
}

bool try_push_incantation_cmd(const char *line,
    player_t *pl, scheduler_t *sched, int freq)
{
    const req_t *req;
    tile_t *tile;

    if (strcmp(line, "Incantation") != 0 || !pl || !pl->world)
        return false;
    req = &REQS[pl->level];
    tile = world_get_tile(pl->world, pl->x, pl->y);
    if (!incantation_allowed(pl, req, tile)) {
        write(pl->fd, "ko\n", 3);
        return true;
    }
    send_incantation_start(pl);
    write(pl->fd, "Elevation underway\n", 20);
    return schedule_incantation(pl, sched, freq);
}
