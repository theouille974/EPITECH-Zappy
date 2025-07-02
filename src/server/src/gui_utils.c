/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** gui_utils.c
*/

#include "gui.h"
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

void sock_printf(int fd, const char *fmt, ...)
{
    char buf[GUI_BUF_SZ];
    va_list ap;
    int n;

    va_start(ap, fmt);
    n = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    if (n > 0)
        write(fd, buf, (size_t)n);
}

void broadcast(net_t *net, const char *msg, size_t n)
{
    player_t *pl;

    for (int i = 1; i < net->nfds; ++i) {
        pl = (player_t *)net->players[i];
        if (IS_GUI(pl))
            write(net->pfds[i].fd, msg, n);
    }
}
