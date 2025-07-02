/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** net_poll
*/

#include "net_poll.h"
#include "player.h"
#include "net_client.h"
#include "gui.h"
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdint.h>
#include <limits.h>
#include "net_utils.h"

static uint64_t now_ms(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000ULL + tv.tv_usec / 1000ULL;
}

static int compute_poll_timeout(const net_t *net, int default_ms)
{
    int wait_ms = default_ms;
    uint64_t now;
    uint64_t until;

    if (net && net->sched) {
        now = now_ms();
        until = scheduler_time_until_next(net->sched, now);
        if (until != UINT64_MAX && until < (uint64_t)wait_ms)
            wait_ms = (int)until;
    }
    if (wait_ms < 0)
        wait_ms = 0;
    if (wait_ms > INT_MAX)
        wait_ms = INT_MAX;
    return wait_ms;
}

void drop_fd(net_t *net, int idx)
{
    player_t *pl = (player_t *)net->players[idx];

    if (pl && pl->authed && pl->team_idx >= 0) {
        gui_broadcast_pdi(net, pl);
        team_release_slot(net->teams, pl->team_idx);
    }
    if (pl && net->sched) {
        scheduler_remove_player_actions(net->sched, pl);
    }
    player_destroy(pl);
    --net->nfds;
    net->players[idx] = net->players[net->nfds];
    net->pfds[idx] = net->pfds[net->nfds];
}

bool net_init(net_t *net, const net_params_t *p)
{
    memset(net, 0, sizeof(*net));
    net->sched = p->sched;
    net->freq = p->freq;
    net->world = p->world;
    net->teams = p->teams;
    net->team_cnt = p->team_cnt;
    net->egg_count = 0;
    net->next_egg_id = 1;
    if (!setup_listen_socket(net, p->port))
        return false;
    net->pfds[0].fd = net->listen_fd;
    net->pfds[0].events = POLLIN;
    net->nfds = 1;
    return true;
}

static void send_welcome(int fd)
{
    const char *msg = "WELCOME\n";

    write(fd, msg, strlen(msg));
}

static void add_client(net_t *net, int fd)
{
    if (net->nfds >= NET_MAX_FDS) {
        close(fd);
        return;
    }
    set_nonblock(fd);
    net->pfds[net->nfds].fd = fd;
    net->pfds[net->nfds].events = POLLIN;
    net->players[net->nfds] = player_create(fd, net->world, net->freq, net);
    if (!net->players[net->nfds]) {
        close(fd);
        return;
    }
    ++net->nfds;
    send_welcome(fd);
}

static void accept_new(net_t *net)
{
    int fd;

    while (1) {
        fd = accept(net->listen_fd, NULL, NULL);
        if (fd < 0)
            return;
        add_client(net, fd);
    }
}

void net_poll_once(net_t *net, int timeout_ms)
{
    int wait_ms = compute_poll_timeout(net, timeout_ms);

    if (poll(net->pfds, net->nfds, wait_ms) <= 0)
        return;
    if (net->pfds[0].revents & POLLIN)
        accept_new(net);
    for (int i = 1; i < net->nfds; ++i) {
        if (net->pfds[i].revents & POLLIN)
            handle_client(net, i);
    }
}

void net_shutdown(net_t *net)
{
    while (net->nfds > 0)
        drop_fd(net, 0);
    close(net->listen_fd);
    memset(net, 0, sizeof(*net));
}
