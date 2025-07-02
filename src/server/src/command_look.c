/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** command look
*/

#include "command_look.h"
#include "player.h"
#include <unistd.h>

static size_t compose_look(const player_t *pl, char *buf, size_t size)
{
    size_t len = 0;
    bool first = true;
    view_ctx_t v;

    buf[len] = '[';
    len++;
    v.buf = buf;
    v.size = size;
    v.len = &len;
    v.first = &first;
    traverse_view(pl, &v);
    if (len < size) {
        buf[len] = ']';
        len++;
    }
    if (len < size) {
        buf[len] = '\n';
        len++;
    }
    return len;
}

void cmd_look(struct s_player *p)
{
    char buf[4096];
    size_t n;

    if (!p || !p->net || !p->world)
        return;
    n = compose_look(p, buf, sizeof(buf));
    write(p->fd, buf, n);
    if (p->q_len > 0)
        --p->q_len;
}
