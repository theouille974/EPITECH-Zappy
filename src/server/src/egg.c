/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** egg
*/

#include "egg.h"
#include "net_poll.h"
#include "gui.h"
#include <string.h>
#include <stddef.h>

static void set_coords(const egg_t *e, int *x, int *y)
{
    if (x)
        *x = e->x;
    if (y)
        *y = e->y;
}

bool egg_create(net_t *net, int x, int y, int team_idx)
{
    egg_t *e;

    if (!net || net->egg_count >= EGG_MAX)
        return false;
    e = &net->eggs[net->egg_count];
    e->id = net->next_egg_id;
    net->next_egg_id += 1;
    e->x = x;
    e->y = y;
    e->team_idx = team_idx;
    net->egg_count += 1;
    return true;
}

bool egg_hatch(net_t *net, int team_idx, int *x, int *y)
{
    int egg_id;

    if (!net || net->egg_count == 0)
        return false;
    for (int i = 0; i < net->egg_count; ++i) {
        if (net->eggs[i].team_idx != team_idx)
            continue;
        egg_id = net->eggs[i].id;
        set_coords(&net->eggs[i], x, y);
        gui_broadcast_ebo(net, egg_id);
        net->eggs[i] = net->eggs[net->egg_count - 1];
        net->egg_count -= 1;
        return true;
    }
    return false;
}
