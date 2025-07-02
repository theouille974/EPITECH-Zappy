/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** net_utils.c - socket helper utilities shared by network modules
*/

#include "net_utils.h"
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>

bool set_nonblock(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);

    if (flags < 0)
        return false;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK) == 0;
}

bool setup_listen_socket(net_t *net, int port)
{
    int opt = 1;
    struct sockaddr_in addr = {0};

    net->listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (net->listen_fd < 0)
        return false;
    setsockopt(net->listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt,
        sizeof(opt));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons((uint16_t)port);
    if (bind(net->listen_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0 ||
        listen(net->listen_fd, SOMAXCONN) < 0 ||
        !set_nonblock(net->listen_fd))
        return false;
    return true;
}
