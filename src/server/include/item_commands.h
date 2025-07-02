/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** net_poll
*/

#include <stdbool.h>
#include "scheduler.h"
#include "player.h"
#include "world.h"

#ifndef ITEM_COMMANDS_H
    #define ITEM_COMMANDS_H

/**
 * @brief Enumeration for item operations.
 * This enum defines the different operations that can be performed on items.
 * - ITEM_OP_TAKE: Represents the operation of taking an item.
 * - ITEM_OP_SET: Represents the operation of setting an item.
 * - ITEM_OP_INVENTORY: Represents the operation of checking the inventory for items.
 */
typedef enum e_item_op {
    ITEM_OP_TAKE,
    ITEM_OP_SET,
    ITEM_OP_INVENTORY
} item_op_t;

/**
 * @brief Structure representing the parameters for an item operation.
 * @param op The operation type (take, set, inventory).
 * @param id The resource ID associated with the item.
 * @note This structure contains the operation type and the resource ID associated with the item.
 */
typedef struct s_item_params {
    item_op_t op;
    res_t id;
} item_params_t;

/**
 * @brief Structure representing the context for an item operation.
 * @param pl Pointer to the player performing the operation.
 * @param id The resource ID associated with the item.
 * @param take Indicates whether the operation is a take action.
 * @note This structure contains the player, resource ID, and whether the operation is a take action.
 */
typedef struct s_item_ctx {
    struct s_player *pl;
    res_t id;
    bool take;
} item_ctx_t;

/**
 * @brief Attempts to push an item command to the scheduler.
 * @param line The command line input.
 * @param pl Pointer to the player performing the operation.
 * @param sched Pointer to the scheduler where the command will be pushed.
 * @param freq The frequency of the scheduler.
 * @return True if the command was successfully pushed, false otherwise.
 * @note This function processes the item command and schedules it for execution.
 */
bool try_push_item_cmd(const char *line,
    struct s_player *pl,
    scheduler_t *sched,
    int freq);

#endif
