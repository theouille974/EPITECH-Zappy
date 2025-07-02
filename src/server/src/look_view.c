/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** look view
*/

#include "command_look.h"
#include "player.h"
#include "world.h"
#include "net_poll.h"

void coord_for_dir(const player_t *pl, int dist, int off,
    pos_t *pos)
{
    switch (pl->dir) {
    case 0:
        pos->x = pl->x + off;
        pos->y = pl->y - dist;
        break;
    case 1:
        pos->x = pl->x + dist;
        pos->y = pl->y + off;
        break;
    case 2:
        pos->x = pl->x - off;
        pos->y = pl->y + dist;
        break;
    default:
        pos->x = pl->x - dist;
        pos->y = pl->y - off;
        break;
    }
}

static void write_sep(view_ctx_t *v)
{
    size_t pos;

    if (*v->first || *v->len + 2 >= v->size)
        return;
    pos = *v->len;
    v->buf[pos] = ',';
    pos++;
    v->buf[pos] = ' ';
    pos++;
    *v->len = pos;
}

void append_tile(const player_t *pl, int tx, int ty, view_ctx_t *v)
{
    buf_ctx_t b;

    write_sep(v);
    *v->first = false;
    b.dst = v->buf + *v->len;
    b.rem = v->size - *v->len;
    b.need_sp = NULL;
    *v->len += build_tile(pl->net, tx, ty, &b);
}

void add_line(const player_t *pl, int dist, view_ctx_t *v)
{
    const world_t *w = pl->world;
    int off;
    pos_t coord;

    for (off = -dist; off <= dist && *v->len < v->size; ++off) {
        coord_for_dir(pl, dist, off, &coord);
        coord.x = wrap_coord(coord.x, w->w);
        coord.y = wrap_coord(coord.y, w->h);
        append_tile(pl, coord.x, coord.y, v);
    }
}

void traverse_view(const player_t *pl, view_ctx_t *v)
{
    int dist;

    for (dist = 0; dist <= pl->level && *v->len < v->size; ++dist)
        add_line(pl, dist, v);
}
