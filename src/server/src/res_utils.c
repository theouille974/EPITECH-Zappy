/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** res_utils
*/

#include "world.h"
#include <string.h>
#include <ctype.h>

static const char *RES_NAMES[RES_MAX] = {
    [RES_FOOD] = "food",
    [RES_LINEMATE] = "linemate",
    [RES_DERAUMERE] = "deraumere",
    [RES_SIBUR] = "sibur",
    [RES_MENDIANE] = "mendiane",
    [RES_PHIRAS] = "phiras",
    [RES_THYSTAME] = "thystame",
};

static bool ieq(const char *a, const char *b)
{
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b))
            return false;
        b++;
        b++;
    }
    return *a == '\0' && *b == '\0';
}

bool res_from_string(const char *name, res_t *out)
{
    if (!name || !out)
        return false;
    for (res_t id = 0; id < RES_MAX; ++id) {
        if (ieq(name, RES_NAMES[id])) {
            *out = id;
            return true;
        }
    }
    return false;
}
