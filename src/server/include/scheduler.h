/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** scheduler
*/

#include <stdbool.h>
#include <stdint.h>

#ifndef SCHEDULER_H
    #define SCHEDULER_H
    #define SCHED_MAX 1024

struct s_player;

typedef void (*cmd_fn_t)(struct s_player *);

/**
 * @brief Structure representing an action in the scheduler.
 * @param exec_at The timestamp when the action should be executed.
 * @param fn The function to be executed for this action.
 * @param pl Pointer to the player associated with this action.
 * @note This structure encapsulates the details of an action that is scheduled to be executed at a specific time.
 * @note It includes the execution time, the function to call, and the player involved in the action.
 */
typedef struct s_action {
    uint64_t exec_at;
    cmd_fn_t fn;
    struct s_player *pl;
} action_t;

/**
 * @brief Structure representing a scheduler that manages actions.
 * @param items An array of actions to be scheduled.
 * @param len The current number of actions in the scheduler.
 * @note This structure is used to manage a collection of actions that need to be executed at specific times.
 * @note It provides functionality to add, remove, and execute actions based on their scheduled time.
 */
typedef struct s_scheduler {
    action_t items[SCHED_MAX];
    int len;
} scheduler_t;

/**
 * @brief Structure representing a command mapping in the scheduler.
 * @param name The name of the command.
 * @param fn The function to be executed for this command.
 * @param cost The cost associated with executing this command.
 * @note This structure is used to map command names to their corresponding functions and execution costs.
 * @note It allows the scheduler to efficiently look up and execute commands based on user input.
 */
typedef struct s_cmd_map {
    const char *name;
    cmd_fn_t fn;
    int cost;
} cmd_map_t;

/**
 * @brief Initializes a scheduler instance.
 * @param s Pointer to the scheduler instance to be initialized.
 * @note This function sets up the scheduler by clearing its action list and preparing it for use.
 */
void scheduler_init(scheduler_t *s);
/**
 * @brief Push a scheduler instance.
 * @param s Pointer to the scheduler instance to be destroyed.
 * @param act The action to be added to the scheduler.
 * @return true if the action was successfully added, false if the scheduler is full.
 */
bool scheduler_push(scheduler_t *s, action_t act);
/**
 * @brief Runs the scheduler, executing actions that are due.
 * @param s Pointer to the scheduler instance.
 * @param now The current timestamp in milliseconds.
 * @note This function checks the scheduler for actions that are due to be executed at the current time.
 * @note It processes each action, calling the associated function and removing it from the scheduler if it is executed.
 */
void scheduler_run_ready(scheduler_t *s, uint64_t now);
/**
 * @brief Removes all actions associated with a specific player from the scheduler.
 * @param s Pointer to the scheduler instance.
 * @param pl Pointer to the player whose actions should be removed.
 * @note This function iterates through the scheduler's action list and removes any actions that are associated with the specified player.
 */
void scheduler_remove_player_actions(scheduler_t *s, struct s_player *pl);
/**
 * @brief Converts a command string to a scheduler action.
 * @param pl Pointer to the player associated with the command.
 * @param line The command string to be parsed.
 * @param s Pointer to the scheduler instance where the action will be added.
 * @param freq The frequency of the player's actions in the game.
 * @return true if the command was successfully parsed and added to the scheduler, false otherwise.
 */
bool sched_cmd_from_string(struct s_player *pl, const char *line,
    scheduler_t *s, int freq);

/* Return the number of milliseconds until the next action is due.
 * If the scheduler is empty, UINT64_MAX is returned. */
/**
 * @brief Calculates the time until the next action is due in the scheduler.
 * @param s Pointer to the scheduler instance.
 * @param now The current timestamp in milliseconds.
 * @return The number of milliseconds until the next action is due, or UINT64_MAX if the scheduler is empty.
 */
uint64_t scheduler_time_until_next(const scheduler_t *s, uint64_t now);

#endif /* SCHEDULER_H */
