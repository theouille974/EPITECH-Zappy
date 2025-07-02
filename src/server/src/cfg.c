/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** cfg
*/

#include "cfg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

static void cfg_defaults(cfg_t *cfg)
{
    memset(cfg, 0, sizeof(*cfg));
    cfg->freq = 100;
}

static bool parse_int(int *out, char *str)
{
    char *end = NULL;
    long v = strtol(str, &end, 10);

    if (!end || *end || v < 0 || v > INT_MAX)
        return false;
    *out = (int)v;
    return true;
}

static bool handle_numeric_flag(int *idx, char **av,
    const char *flag, int *dst)
{
    int i = *idx;

    if (!strcmp(av[i], flag) && av[i + 1] &&
        parse_int(dst, av[i + 1])) {
        *idx += 1;
        return true;
    }
    return false;
}

static bool handle_teams_flag(int *idx, int ac, char **av, cfg_t *cfg)
{
    int i = *idx;

    if (strcmp(av[i], "-n"))
        return false;
    cfg->teams = &av[i + 1];
    while (i + 1 < ac && av[i + 1][0] != '-') {
        ++cfg->team_count;
        ++i;
    }
    *idx = i;
    return true;
}

static bool handle_flag(int *idx, int ac, char **av, cfg_t *cfg)
{
    return handle_numeric_flag(idx, av, "-p", &cfg->port) ||
        handle_numeric_flag(idx, av, "-x", &cfg->width) ||
        handle_numeric_flag(idx, av, "-y", &cfg->height) ||
        handle_numeric_flag(idx, av, "-c", &cfg->clients_nb) ||
        handle_numeric_flag(idx, av, "-f", &cfg->freq) ||
        handle_teams_flag(idx, ac, av, cfg);
}

bool cfg_parse(cfg_t *cfg, int ac, char **av)
{
    cfg_defaults(cfg);
    for (int i = 1; i < ac; ++i) {
        if (!handle_flag(&i, ac, av, cfg))
            return false;
    }
    return cfg->port && cfg->width && cfg->height &&
        cfg->team_count && cfg->clients_nb && cfg->freq;
}

void cfg_free(cfg_t *cfg)
{
    (void)cfg;
}
