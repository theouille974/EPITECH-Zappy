/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** world
*/

#include "world.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

static const double DENS[RES_MAX] = {0.5, 0.3, 0.15, 0.1, 0.1, 0.08, 0.05};

static void place_once(world_t *w, res_t id)
{
    int idx = rand() % (w->w * w->h);

    w->tiles[idx].res[id]++;
}

static void spawn_resources(world_t *w)
{
    int area = w->w * w->h;
    int n = 0;

    for (res_t id = 0; id < RES_MAX; ++id) {
        n = (int)(area * DENS[id] + 0.5);
        if (n < 1)
            n = 1;
        for (int i = 0; i < n; ++i)
            place_once(w, id);
    }
}

bool world_create(world_t *w, const cfg_t *cfg)
{
    memset(w, 0, sizeof(*w));
    w->w = cfg->width;
    w->h = cfg->height;
    w->tiles = calloc((size_t)w->w * w->h, sizeof(tile_t));
    if (!w->tiles)
        return false;
    srand((unsigned)time(NULL));
    spawn_resources(w);
    return true;
}

void world_destroy(world_t *w)
{
    free(w->tiles);
    memset(w, 0, sizeof(*w));
}
