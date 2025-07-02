/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** team
*/

#include <stdbool.h>

#ifndef TEAM_H
    #define TEAM_H

/**
 * @brief Structure representing a team in the game.
 * @param name The name of the team.
 * @param slots The number of available slots for players in the team.
 * @note This structure is used to manage teams within the game, allowing for team identification and
 *       tracking of available player slots.
 */
typedef struct s_team {
    char *name;
    int slots;
} team_t;

/**
 * @brief Initializes an array of teams.
 * @param arr Pointer to the array of team structures.
 * @param count The number of teams to initialize.
 * @param names Array of team names.
 * @param slots The number of slots available for each team.
 * @note This function sets up the teams with their names and available slots, preparing them for gameplay.
 */
void teams_init(team_t *arr, int count, char **names, int slots);
/**
 * @brief Finds a team by its name in the array of teams.
 * @param arr Pointer to the array of team structures.
 * @param count The number of teams in the array.
 * @param name The name of the team to find.
 * @return The index of the team if found, or -1 if not found.
 * @note This function is used to locate a specific team based on its name, allowing for operations on that team.
 */
int team_find(team_t *arr, int count, const char *name);
/**
 * @brief Attempts to take a slot in a team for a new player.
 * @param arr Pointer to the array of team structures.
 * @param count The number of teams in the array.
 * @param name The name of the team to take a slot in.
 * @param remaining Pointer to an integer that will hold the number of remaining slots after taking one.
 * @return True if a slot was successfully taken, false if no slots are available.
 */
bool team_take_slot(team_t *arr, int count, const char *name, int *remaining);
/**
 * @brief Releases a slot in a team, making it available for new players.
 * @param arr Pointer to the array of team structures.
 * @param idx The index of the team in the array whose slot is to be released.
 * @note This function is used to free up a slot in a team, allowing other players to join the team.
 */
void team_release_slot(team_t *arr, int idx);

#endif /* TEAM_H */
