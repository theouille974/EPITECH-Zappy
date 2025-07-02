/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** command_handler
*/

#ifndef COMMAND_HANDLERS_H
    #define COMMAND_HANDLERS_H

struct s_player;

/**
 * @brief Handles the "Forward" command for a player.
 * @param p Pointer to the player structure.
 * @note This function moves the player forward in the game world.
 */
void cmd_forward(struct s_player *p);
/**
 * @brief Handles the "Right" command for a player.
 * @param p Pointer to the player structure.
 * @note This function rotates the player to the right in the game world.
 */
void cmd_right(struct s_player *p);
/**
 * @brief Handles the "Left" command for a player.
 * @param p Pointer to the player structure.
 * @note This function rotates the player to the left in the game world.
 */
void cmd_left(struct s_player *p);
/**
 * @brief Handles the "Look" command for a player.
 * @param p Pointer to the player structure.
 * @note This function allows the player to look around in the game world.
 */
void cmd_connect_nbr(struct s_player *p);
/**
 * @brief Handles the "Inventory" command for a player.
 * @param p Pointer to the player structure.
 * @note This function retrieves the player's inventory in the game world.
 */
void cmd_broadcast(struct s_player *p);
/**
 * @brief Handles the "Take" command for a player.
 * @param p Pointer to the player structure.
 * @note This function allows the player to take an item from the game world.
 */
void cmd_eject(struct s_player *p);
/**
 * @brief Handles the "Set" command for a player.
 * @param p Pointer to the player structure.
 * @note This function allows the player to set an item in the game world.
 */
void cmd_fork(struct s_player *p);

#endif /* COMMAND_HANDLERS_H */
