/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** net_client - client connection and authentication helpers
*/

#include "net_client.h"
#include "player.h"
#include "team.h"
#include "gui.h"
#include "egg.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdint.h>

static void trim_cr(player_t *pl)
{
    size_t len = strlen(pl->buf);

    if (len && pl->buf[len - 1] == '\r') {
        --len;
        pl->buf[len] = '\0';
    }
}

static void skip_consumed(player_t *pl, char *nl)
{
    size_t consumed = (nl - pl->buf) + 1;

    memmove(pl->buf, pl->buf + consumed, pl->len - consumed);
    pl->len -= consumed;
}

static void __attribute__((unused)) send_graphic_welcome(player_t *pl,
    const struct s_world *world)
{
    char line[64];

    snprintf(line, sizeof(line), "%d %d\n", world->w, world->h);
    write(pl->fd, line, strlen(line));
    pl->authed = true;
}

static void handle_graphic(net_t *net, player_t *pl)
{
    pl->team_idx = -2;
    pl->authed = true;
    gui_send_initial(net, pl->fd);
}

static void handle_team_line(net_t *net, int idx, player_t *pl, char *nl)
{
    char *team_name = pl->buf;

    *nl = '\0';
    trim_cr(pl);
    if (!strcmp(team_name, "GRAPHIC"))
        handle_graphic(net, pl);
    else if (!assign_team(net, idx, pl, team_name))
        return;
    skip_consumed(pl, nl);
}

static void authenticate_player(net_t *net, int idx, const char *buf,
    ssize_t r)
{
    player_t *pl = (player_t *)net->players[idx];
    char *nl;

    if (pl->len + (size_t)r >= PLAYER_BUF_SZ) {
        drop_fd(net, idx);
        return;
    }
    memcpy(pl->buf + pl->len, buf, (size_t)r);
    pl->len += (size_t)r;
    nl = memchr(pl->buf, '\n', pl->len);
    if (!nl)
        return;
    handle_team_line(net, idx, pl, nl);
}

void handle_client(net_t *net, int idx)
{
    player_t *pl = (player_t *)net->players[idx];
    char buf[256];
    ssize_t r = read(net->pfds[idx].fd, buf, sizeof(buf));

    if (r <= 0) {
        drop_fd(net, idx);
        return;
    }
    if (!pl->authed) {
        authenticate_player(net, idx, buf, r);
        return;
    }
    player_feed(pl, buf, (size_t)r, net->sched);
}
