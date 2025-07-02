/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** incantation.h
*/

#include <stdbool.h>
#include "scheduler.h"
#include "player.h"
#include "world.h"
#include "net_poll.h"

#ifndef INCANTATION_H
    #define INCANTATION_H
    #define INCANTATION_DELAY 300

/**
 * @brief Structure representing the requirements for an incantation.
 * @param players The number of players required for the incantation.
 * @param stones An array representing the required resources for the incantation.
 */
typedef struct req_s {
    int players;
    int stones[RES_MAX];
} req_t;

/**
 * @brief Structure representing the context for an incantation.
 * @param initiator Pointer to the player initiating the incantation.
 * @param x The X coordinate of the tile where the incantation is taking place.
 * @param y The Y coordinate of the tile where the incantation is taking place.
 * @param level The level of the incantation.
 */
typedef struct inc_ctx_s {
    player_t *initiator;
    int x;
    int y;
    int level;
} inc_ctx_t;

/**
 * @brief Structure representing the result context of an incantation.
 * @param net Pointer to the network structure containing the game state.
 * @param ctx Pointer to the incantation context.
 * @param tile Pointer to the tile where the incantation is taking place.
 * @param init Pointer to the player who initiated the incantation.
 * @param success Indicates whether the incantation was successful.
 */
typedef struct inc_result_ctx_s {
    net_t *net;
    inc_ctx_t *ctx;
    tile_t *tile;
    player_t *init;
    int success;
} inc_result_ctx_t;

/**
 * @brief Executes the incantation command.
 * @param ctx Pointer to the incantation context.
 * @return True if the incantation was executed successfully, false otherwise.
 */
bool try_push_incantation_cmd(const char *line,
    struct s_player *pl,
    scheduler_t *sched,
    int freq);

extern const req_t REQS[8];

/**
 * @brief Schedules an incantation for a player.
 * @param pl Pointer to the player initiating the incantation.
 * @param sched Pointer to the scheduler where the incantation will be scheduled.
 * @param freq The frequency of the scheduler.
 * @return True if the incantation was successfully scheduled, false otherwise.
 */
bool schedule_incantation(struct s_player *pl,
    scheduler_t *sched,
    int freq);

#endif /* INCANTATION_H */
