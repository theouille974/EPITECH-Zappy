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
#include <stdbool.h>

static void reset_hunger(player_t *pl)
{
    struct timeval tv;
    uint64_t now_ms;

    gettimeofday(&tv, NULL);
    now_ms = (uint64_t)tv.tv_sec * 1000ULL + tv.tv_usec / 1000ULL;
    pl->next_food = now_ms + 126000ULL / (uint64_t)pl->freq;
}

static void send_ko_and_drop(player_t *pl, net_t *net, int idx)
{
    const char *ko = "ko\n";

    write(pl->fd, ko, strlen(ko));
    drop_fd(net, idx);
}

static void setup_player_auth(player_t *pl, int team_idx)
{
    pl->team_idx = team_idx;
    pl->authed = true;
    reset_hunger(pl);
}

static void send_join_ack(net_t *net, player_t *pl, int remaining)
{
    char buf[64];

    snprintf(buf, sizeof(buf), "%d\n", remaining);
    write(pl->fd, buf, strlen(buf));
    snprintf(buf, sizeof(buf), "%d %d\n", net->world->w, net->world->h);
    write(pl->fd, buf, strlen(buf));
    gui_broadcast_pnw(net, pl);
    gui_broadcast_pin(net, pl);
}

static bool handle_egg_hatch(net_t *net, int team_idx, player_t *pl)
{
    if (!egg_hatch(net, team_idx, &pl->x, &pl->y))
        return false;
    setup_player_auth(pl, team_idx);
    send_join_ack(net, pl, net->teams[team_idx].slots);
    return true;
}

static bool handle_team_slot(net_t *net, int idx, player_t *pl,
    const char *team_name)
{
    int remaining = 0;
    int team_idx = team_find(net->teams, net->team_cnt, team_name);

    if (!team_take_slot(net->teams, net->team_cnt, team_name, &remaining)) {
        send_ko_and_drop(pl, net, idx);
        return false;
    }
    setup_player_auth(pl, team_idx);
    send_join_ack(net, pl, remaining);
    return true;
}

bool assign_team(net_t *net, int idx, player_t *pl, const char *team_name)
{
    int team_idx = team_find(net->teams, net->team_cnt, team_name);

    if (team_idx < 0) {
        send_ko_and_drop(pl, net, idx);
        return false;
    }
    if (handle_egg_hatch(net, team_idx, pl))
        return true;
    return handle_team_slot(net, idx, pl, team_name);
}
