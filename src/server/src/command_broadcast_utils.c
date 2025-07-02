/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** command_broadcast_utils.c
*/

#include "command_broadcast_utils.h"
#include <stdlib.h>
#include <string.h>

char *extract_broadcast_text(const char *line, const char *prefix)
{
    size_t plen = strlen(prefix);

    if (strncmp(line, prefix, plen) != 0)
        return NULL;
    if (line[plen] == '\0')
        return strdup("");
    if (line[plen] == ' ')
        return strdup(line + plen + 1);
    return NULL;
}

broadcast_ctx_t *create_broadcast_ctx(player_t *pl, char *text)
{
    broadcast_ctx_t *ctx = malloc(sizeof(*ctx));

    if (!ctx)
        return NULL;
    ctx->pl = pl;
    ctx->msg = text;
    return ctx;
}
