/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** commands
*/

#include "scheduler.h"
#include "player.h"
#include "world.h"
#include "net_poll.h"
#include "gui.h"
#include "command_look.h"
#include "item_commands.h"
#include "command_handlers.h"
#include "command_broadcast.h"
#include "incantation.h"
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

static const cmd_map_t CMD_MAP[] = {
    {"Forward", cmd_forward, 7},
    {"Right", cmd_right, 7},
    {"Left", cmd_left, 7},
    {"Look", cmd_look, 7},
    {"Eject", cmd_eject, 7},
    {"Fork", cmd_fork, 42},
    {"Connect_nbr", cmd_connect_nbr, 0},
};

static uint64_t now_ms(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000ULL + tv.tv_usec / 1000ULL;
}

void cmd_forward(struct s_player *p)
{
    static const int DX[4] = {0, 1, 0, -1};
    static const int DY[4] = {-1, 0, 1, 0};
    const char *msg = "ok\n";

    p->x = (p->x + DX[p->dir] + p->world->w) % p->world->w;
    p->y = (p->y + DY[p->dir] + p->world->h) % p->world->h;
    write(p->fd, msg, strlen(msg));
    if (p->net)
        gui_broadcast_ppo(p->net, p);
    if (p->q_len > 0)
        --p->q_len;
}

void cmd_right(struct s_player *p)
{
    const char *msg = "ok\n";

    p->dir = (p->dir + 1) % 4;
    write(p->fd, msg, strlen(msg));
    if (p->net)
        gui_broadcast_ppo(p->net, p);
    if (p->q_len > 0)
        --p->q_len;
}

void cmd_left(struct s_player *p)
{
    const char *msg = "ok\n";

    p->dir = (p->dir + 3) % 4;
    write(p->fd, msg, strlen(msg));
    if (p->net)
        gui_broadcast_ppo(p->net, p);
    if (p->q_len > 0)
        --p->q_len;
}

void cmd_connect_nbr(struct s_player *p)
{
    char buf[32];
    int slots = 0;

    if (p->team_idx >= 0 && p->net && p->net->teams)
        slots = p->net->teams[p->team_idx].slots;
    snprintf(buf, sizeof(buf), "%d\n", slots);
    write(p->fd, buf, strlen(buf));
    if (p->q_len > 0)
        --p->q_len;
}

static const cmd_map_t *find_command(const char *name)
{
    for (size_t i = 0; i < sizeof(CMD_MAP) / sizeof(*CMD_MAP); ++i)
        if (!strcmp(name, CMD_MAP[i].name))
            return &CMD_MAP[i];
    return NULL;
}

static bool try_push_cmd(const char *line, struct s_player *pl,
    scheduler_t *sched, int freq)
{
    const cmd_map_t *cmd = find_command(line);
    action_t act = {0};

    if (!cmd)
        return false;
    act.exec_at = now_ms() + (cmd->cost * 1000ULL) / (uint64_t)freq;
    act.fn = cmd->fn;
    act.pl = pl;
    if (!scheduler_push(sched, act))
        return false;
    pl->q_len += 1;
    return true;
}

bool sched_cmd_from_string(struct s_player *pl, const char *line,
    scheduler_t *s, int freq)
{
    const char *ko = "ko\n";

    if (try_push_cmd(line, pl, s, freq) ||
        try_push_item_cmd(line, pl, s, freq) ||
        try_push_broadcast_cmd(line, pl, s, freq) ||
        try_push_incantation_cmd(line, pl, s, freq))
        return true;
    write(pl->fd, ko, strlen(ko));
    return false;
}
