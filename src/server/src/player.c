/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** player
*/

#include "player.h"
#include "scheduler.h"
#include "world.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdint.h>

static uint64_t now_ms(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000ULL + tv.tv_usec / 1000ULL;
}

static void player_setup(player_t *p, int fd, struct s_world *world, int freq)
{
    p->fd = fd;
    p->world = world;
    if (world) {
        p->x = rand() % world->w;
        p->y = rand() % world->h;
    }
    p->dir = rand() % 4;
    p->level = 1;
    p->authed = false;
    p->team_idx = -1;
    memset(p->inv, 0, sizeof(p->inv));
    p->inv[RES_FOOD] = 10;
    p->freq = freq;
    p->next_food = now_ms() + 126000ULL / (uint64_t)freq;
}

player_t *player_create(int fd, struct s_world *world,
    int freq, struct s_net *net)
{
    player_t *p = calloc(1, sizeof(*p));

    if (!p)
        return NULL;
    player_setup(p, fd, world, freq);
    p->net = net;
    return p;
}

void player_destroy(player_t *p)
{
    if (!p)
        return;
    for (int i = 0; i < p->q_len; ++i)
        free(p->queue[i]);
    close(p->fd);
    free(p);
}

static void enqueue_cmd(player_t *p, const char *line,
    scheduler_t *sched)
{
    const char *ko;

    if (p->q_len >= PLAYER_QUEUE_MAX) {
        ko = "ko\n";
        write(p->fd, ko, strlen(ko));
        return;
    }
    sched_cmd_from_string(p, line, sched, p->freq);
}

static void handle_line(player_t *p, size_t start, size_t end,
    scheduler_t *sched)
{
    if (end > start && p->buf[end - 1] == '\r')
        p->buf[end - 1] = '\0';
    enqueue_cmd(p, p->buf + start, sched);
}

bool player_feed(player_t *p, const char *data, size_t n,
    scheduler_t *sched)
{
    size_t start;
    size_t i;

    if (p->len + n >= PLAYER_BUF_SZ)
        return false;
    memcpy(p->buf + p->len, data, n);
    p->len += n;
    start = 0;
    for (i = 0; i < p->len; ++i) {
        if (p->buf[i] == '\n') {
            p->buf[i] = '\0';
            handle_line(p, start, i, sched);
            start = i + 1;
        }
    }
    if (start) {
        memmove(p->buf, p->buf + start, p->len - start);
        p->len -= start;
    }
    return true;
}
