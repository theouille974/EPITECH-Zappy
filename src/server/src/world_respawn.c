/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** world_respawn
*/

#include "world.h"
#include <stdlib.h>

static const double DENS[RES_MAX] = {0.5, 0.3, 0.15, 0.1, 0.1, 0.08, 0.05};

static void place_once(world_t *w, res_t id)
{
    int idx = rand() % (w->w * w->h);

    w->tiles[idx].res[id]++;
}

static int count_res(const world_t *w, res_t id)
{
    int total = 0;

    for (int i = 0; i < w->w * w->h; ++i)
        total += w->tiles[i].res[id];
    return total;
}

static void ensure_minimum_resources(world_t *w)
{
    for (res_t id = 0; id < RES_MAX; ++id) {
        if (count_res(w, id) == 0) {
            place_once(w, id);
        }
    }
}

static void spawn_resources_by_density(world_t *w, double density_factor)
{
    int area = w->w * w->h;
    int target = 0;
    int have = 0;
    int need = 0;

    if (area <= 0)
        return;
    for (res_t id = 0; id < RES_MAX; ++id) {
        target = (int)(area * DENS[id] * density_factor + 0.5);
        if (target < 1)
            target = 1;
        have = count_res(w, id);
        need = target - have;
        spawn_resources_if_needed(w, id, need);
    }
}

void spawn_resources_if_needed(world_t *w, res_t id, int need)
{
    if (need <= 0)
        return;
    for (int i = 0; i < need; ++i)
        place_once(w, id);
}

void world_respawn(world_t *w)
{
    spawn_resources_by_density(w, 1.0);
    ensure_minimum_resources(w);
}

void world_periodic_refill(world_t *w)
{
    spawn_resources_by_density(w, 1.0);
    ensure_minimum_resources(w);
}
