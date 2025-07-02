/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** command fork
*/

#include "command_handlers.h"
#include "egg.h"
#include "player.h"
#include "net_poll.h"
#include "team.h"
#include "gui.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>

void cmd_fork(struct s_player *p)
{
    const char *OK = "ok\n";
    egg_t *e;
    gui_enw_t enw;

    if (!p || !p->net || p->team_idx < 0)
        return;
    if (egg_create(p->net, p->x, p->y, p->team_idx)) {
        p->net->teams[p->team_idx].slots += 1;
        e = &p->net->eggs[p->net->egg_count - 1];
        gui_broadcast_pfk(p->net, p);
        enw.egg_id = e->id;
        enw.player_id = p->fd;
        enw.x = e->x;
        enw.y = e->y;
        gui_broadcast_enw(p->net, &enw);
    }
    write(p->fd, OK, strlen(OK));
    if (p->q_len > 0)
        --p->q_len;
}
