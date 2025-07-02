/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** gui_broadcast_incant.c
*/

#include "gui.h"
#include <stdio.h>
#include <unistd.h>

static size_t write_pic_prefix(char *buf, const gui_pic_t *info)
{
    return (size_t)snprintf(buf, PIC_BUF_SZ, "pic %d %d %d",
        info->x, info->y, info->level);
}

static size_t add_pic_ids(char *buf, size_t len, const gui_pic_t *info)
{
    int n;

    for (size_t i = 0; i < info->count && len < PIC_BUF_SZ - 10; ++i) {
        n = snprintf(buf + len, PIC_BUF_SZ - len, " #%d", info->ids[i]);
        if (n < 0)
            break;
        len += (size_t)n;
    }
    return len;
}

void gui_broadcast_pie(net_t *net, int x, int y, int result)
{
    char line[GUI_BUF_SZ];
    int n = snprintf(line, sizeof(line), "pie %d %d %d\n", x, y, result);

    if (n < 0)
        return;
    broadcast(net, line, (size_t)n);
}

void gui_broadcast_pic(net_t *net, const gui_pic_t *info)
{
    char buf[PIC_BUF_SZ];
    size_t len;

    if (!info || !net)
        return;
    len = write_pic_prefix(buf, info);
    if (len >= PIC_BUF_SZ)
        return;
    len = add_pic_ids(buf, len, info);
    if (len >= PIC_BUF_SZ - 1)
        len = PIC_BUF_SZ - 2;
    buf[len] = '\n';
    len++;
    broadcast(net, buf, len);
}
