/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** item_helpers.c - common helper util for item commands
*/

#include "item_helpers.h"
#include "gui.h"
#include <sys/time.h>

static const char *RES_NAMES[RES_MAX] = {
    "food", "linemate", "deraumere", "sibur",
    "mendiane", "phiras", "thystame"
};

uint64_t ih_now_ms(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000ULL + tv.tv_usec / 1000ULL;
}

void ih_reply(int fd, const char *msg)
{
    write(fd, msg, strlen(msg));
}

bool ih_res_from_string(const char *word, res_t *out)
{
    for (res_t r = 0; r < RES_MAX; ++r) {
        if (ih_str_eq(word, RES_NAMES[r])) {
            *out = r;
            return true;
        }
    }
    return false;
}

bool ih_perform_take(player_t *p, res_t id, tile_t *t)
{
    if (t->res[id] == 0)
        return false;
    --t->res[id];
    ++p->inv[id];
    return true;
}

bool ih_perform_set(player_t *p, res_t id, tile_t *t)
{
    if (p->inv[id] == 0)
        return false;
    --p->inv[id];
    ++t->res[id];
    return true;
}
