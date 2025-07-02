/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** main
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdint.h>
#include <unistd.h>
#include "cfg.h"
#include "world.h"
#include "net_poll.h"
#include "scheduler.h"
#include "team.h"
#include "gui.h"
#include "net_client.h"
#include "player.h"
#include "hunger.h"

static uint64_t now_ms(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000ULL + tv.tv_usec / 1000ULL;
}

static void print_usage(const char *prog)
{
    printf("USAGE: %s -p port -x width -y height -n name1 name2"
        " -c clientsNb -f freq\n", prog);
}

static int cleanup_world_teams(world_t *world, team_t *teams, const char *msg)
{
    if (msg)
        fprintf(stderr, "%s\n", msg);
    world_destroy(world);
    free(teams);
    return EXIT_FAILURE;
}

static void run_loop(net_t *net, scheduler_t *sched)
{
    uint64_t now;
    int poll_timeout;
    uint64_t next_event;

    while (1) {
        now = now_ms();
        next_event = scheduler_time_until_next(sched, now);
        poll_timeout = (next_event > 50) ? 50 : (int)next_event;
        net_poll_once(net, poll_timeout);
        now = now_ms();
        hunger_check(net, now);
        scheduler_run_ready(sched, now);
    }
}

void schedule_periodic_refill(net_t *net, scheduler_t *sched)
{
    action_t act = {0};
    uint64_t now = now_ms();
    uint64_t period = 20000ULL / (uint64_t)net->freq;

    act.exec_at = now + period;
    act.fn = exec_periodic_refill;
    act.pl = (player_t *)net;
    scheduler_push(sched, act);
}

void exec_periodic_refill(struct s_player *raw)
{
    net_t *net = (net_t *)raw;

    if (!net || !net->world)
        return;
    world_periodic_refill(net->world);
    gui_broadcast_full_map(net);
    schedule_periodic_refill(net, net->sched);
}

static int init_server_components(const cfg_t *cfg,
    server_components_t *components)
{
    net_params_t np;

    if (!world_create(components->world, cfg))
        return EXIT_FAILURE;
    scheduler_init(components->sched);
    *components->teams = calloc(cfg->team_count, sizeof(**components->teams));
    if (!*components->teams)
        return EXIT_FAILURE;
    teams_init(*components->teams,
        cfg->team_count, cfg->teams, cfg->clients_nb);
    np = (net_params_t){.port = cfg->port, .world = components->world,
        .teams = *components->teams, .team_cnt = cfg->team_count,
        .sched = components->sched, .freq = cfg->freq};
    if (!net_init(components->net, &np))
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

static void cleanup_server_components(world_t *world,
    team_t *teams, net_t *net)
{
    net_shutdown(net);
    world_destroy(world);
    free(teams);
}

static int run_server(const cfg_t *cfg)
{
    world_t world;
    scheduler_t sched;
    team_t *teams;
    net_t net;
    server_components_t components = {
        .world = &world,
        .sched = &sched,
        .teams = &teams,
        .net = &net
    };

    if (init_server_components(cfg, &components) != EXIT_SUCCESS)
        return cleanup_world_teams(&world, teams, "Failed to init components");
    schedule_periodic_refill(&net, &sched);
    run_loop(&net, &sched);
    cleanup_server_components(&world, teams, &net);
    return EXIT_SUCCESS;
}

int main(int ac, char **av)
{
    cfg_t cfg;
    int ret;

    if (!cfg_parse(&cfg, ac, av)) {
        print_usage(av[0]);
        return EXIT_FAILURE;
    }
    ret = run_server(&cfg);
    cfg_free(&cfg);
    return ret;
}
