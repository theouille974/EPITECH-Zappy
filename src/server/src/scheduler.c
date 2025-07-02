/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** scheduler
*/

#include "scheduler.h"
#include "player.h"
#include "world.h"
#include <string.h>
#include <unistd.h>
#include <stdio.h>

void scheduler_init(scheduler_t *s)
{
    s->len = 0;
}

static void swap(action_t *a, action_t *b)
{
    action_t tmp = *a;

    *a = *b;
    *b = tmp;
}

bool scheduler_push(scheduler_t *s, action_t act)
{
    int i;

    if (s->len >= SCHED_MAX)
        return false;
    i = s->len;
    s->len += 1;
    s->items[i] = act;
    while (i && s->items[i].exec_at < s->items[(i - 1) / 2].exec_at) {
        swap(&s->items[i], &s->items[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
    return true;
}

/* Restore heap property from index i downward */
static void bubble_down(scheduler_t *s, int i)
{
    int l;
    int r;
    int smallest;

    while (1) {
        l = 2 * i + 1;
        r = l + 1;
        smallest = i;
        if (l < s->len && s->items[l].exec_at < s->items[smallest].exec_at)
            smallest = l;
        if (r < s->len && s->items[r].exec_at < s->items[smallest].exec_at)
            smallest = r;
        if (smallest == i)
            return;
        swap(&s->items[i], &s->items[smallest]);
        i = smallest;
    }
}

static void heap_pop(scheduler_t *s)
{
    if (!s->len)
        return;
    s->len -= 1;
    s->items[0] = s->items[s->len];
    bubble_down(s, 0);
}

void scheduler_run_ready(scheduler_t *s, uint64_t now)
{
    action_t act;

    while (s->len && s->items[0].exec_at <= now) {
        act = s->items[0];
        heap_pop(s);
        act.fn(act.pl);
    }
}

static void copy_item_if_needed(scheduler_t *s,
    int i, int *write_pos, struct s_player *pl)
{
    if (s->items[i].pl != pl) {
        if (*write_pos != i) {
            s->items[*write_pos] = s->items[i];
        }
        (*write_pos)++;
    }
}

void scheduler_remove_player_actions(scheduler_t *s, struct s_player *pl)
{
    int write_pos;

    if (!s || !pl)
        return;
    write_pos = 0;
    for (int i = 0; i < s->len; ++i) {
        copy_item_if_needed(s, i, &write_pos, pl);
    }
    s->len = write_pos;
    for (int i = s->len / 2 - 1; i >= 0; --i) {
        bubble_down(s, i);
    }
}

uint64_t scheduler_time_until_next(const scheduler_t *s, uint64_t now)
{
    if (!s || s->len == 0)
        return UINT64_MAX;
    if (s->items[0].exec_at <= now)
        return 0;
    return s->items[0].exec_at - now;
}
