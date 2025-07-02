/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** gui.h
*/

#include "net_poll.h"
#include "player.h"
#include <stddef.h>

#ifndef GUI_H
    #define GUI_H

    #define GUI_BUF_SZ 128
    #define IS_GUI(p) ((p) && (p)->authed && (p)->team_idx == -2)
    #define PIC_BUF_SZ 256

/**
 * @brief Sends the initial map and player information to the GUI client.
 * @param net Pointer to the network structure containing the game state.
 * @param fd The file descriptor of the GUI client socket.
 * @note This function sends the full map and player information to the GUI client upon connection.
 */
void gui_send_initial(net_t *net, int fd);
/**
 * @brief Broadcasts the full map to all GUI clients.
 * @param net Pointer to the network structure containing the game state.
 * @note This function sends the complete map representation to all connected GUI clients.
 * @note It includes the positions of players, resources, and tiles in the game world.
 */
void gui_broadcast_full_map(net_t *net);
/**
 * @brief Broadcasts the player information to all GUI clients.
 * @param net Pointer to the network structure containing the game state.
 * @param pl Pointer to the player structure containing player information.
 * @note This function sends the details of a specific player to all connected GUI clients.
 */
void gui_broadcast_ppo(net_t *net, const player_t *pl);
/**
 * @brief Broadcasts the tile information at a specific position to all GUI clients.
 * @param net Pointer to the network structure containing the game state.
 * @param x The X coordinate of the tile.
 * @param y The Y coordinate of the tile.
 * @note This function sends the details of a specific tile, including resources and players on it, to all connected GUI clients.
 */
void gui_broadcast_tile(net_t *net, int x, int y);
/**
 * @brief Broadcasts the new's player position and direction to all GUI clients.
 * @param net Pointer to the network structure containing the game state.
 * @param pl Pointer to the player structure containing player position and direction information.
 * @note This function sends the information about a newly connected player to all connected GUI clients.
 */
void gui_broadcast_pnw(net_t *net, const player_t *pl);
/**
 * @brief Broadcasts the player inventory to all GUI clients.
 * @param net Pointer to the network structure containing the game state.
 * @param pl Pointer to the player structure containing player inventory information.
 * @note This function sends the inventory details of a specific player to all connected GUI clients.
 */
void gui_broadcast_pin(net_t *net, const player_t *pl);
/**
 * @brief Broadcasts the player death information to all GUI clients.
 * @param net Pointer to the network structure containing the game state.
 * @param pl Pointer to the player structure containing player death information.
 * @note This function sends a notification about a player's death to all connected GUI clients.
 */
void gui_broadcast_pdi(net_t *net, const player_t *pl);
/**
 * @brief Broadcasts the ressources collecting information to all GUI clients.
 * @param net Pointer to the network structure containing the game state.
 * @param pl Pointer to the player structure containing player death information.
 * @param res Result of the GUI operation or command execution.
 */
void gui_broadcast_pgt(net_t *net, const player_t *pl, res_t res);
/**
 * @brief Broadcats the ressource dropping information to all GUI clients.
 * @param net Pointer to the network structure containing the game state.
 * @param pl Pointer to the player structure containing player death information.
 * @param res Result of the GUI operation or command execution.
 */
void gui_broadcast_pdr(net_t *net, const player_t *pl, res_t res);
/**
 * @brief Refresh the content of the map and inventory of the player.
 * @param net Pointer to the network structure containing the game state.
 * @param pl Pointer to the player structure containing player death information.
 */
void gui_refresh_bct_pin(net_t *net, const player_t *pl);
/**
 * @brief Broadcats the broadcast message to all GUI clients.
 * @param net Pointer to the network structure containing the game state.
 * @param pl Pointer to the player structure containing player death information.
 * @param text The broadcast message to be sent.
 * @note This function sends a broadcast message from a player to all connected GUI clients.
 */
void gui_broadcast_pbc(net_t *net, const player_t *pl, const char *text);
/**
 * @brief Broadcasts the player expulsion information to all GUI clients.
 * @param net Pointer to the network structure containing the game state.
 * @param pl Pointer to the player structure containing player expulsion information.
 * @note This function sends a notification about a player's expulsion from the game to all connected GUI clients.
 */
void gui_broadcast_pex(net_t *net, const player_t *pl);
/**
 * @brief Broadcasts the egg laying by the player information to all GUI clients.
 * @param net Pointer to the network structure containing the game state.
 * @param pl Pointer to the player structure containing player level up information.
 * @note This function sends a notification about a player's egg to all connected GUI clients.
 */
void gui_broadcast_pfk(net_t *net, const player_t *pl);
/**
 * @brief Broadcasts the player level up information to all GUI clients.
 * @param net Pointer to the network structure containing the game state.
 * @param pl Pointer to the player structure containing player level up information.
 * @note This function sends a notification about a player's level up to all connected GUI clients.
 */
void gui_broadcast_plv(net_t *net, const player_t *pl);
/**
 * @brief Broadcasts the end of an incantation information to all GUI clients.
 * @param net Pointer to the network structure containing the game state.
 * @param pl Pointer to the player structure containing player connection information.
 * @note This function sends a notification about a player's connection to all connected GUI clients.
 */
void gui_broadcast_pie(net_t *net, int x, int y, int result);

/**
 * @brief Socket printf function to send formatted data to a socket.
 * @param fd The file descriptor of the socket.
 * @param fmt The format string for the data to be sent.
 */
void sock_printf(int fd, const char *fmt, ...);
/**
 * @brief Broadcasts a message to all GUI clients.
 * @param net Pointer to the network structure containing the game state.
 * @param msg The message to be broadcasted.
 * @param n The length of the message.
 * @note This function sends a message to all connected GUI clients.
 */
void broadcast(net_t *net, const char *msg, size_t n);

/**
 * @brief Structure representing a picture to be sent to the GUI.
 * @param x The X coordinate of the picture.
 * @param y The Y coordinate of the picture.
 * @param level The level of the picture.
 * @param ids Pointer to an array of resource IDs associated with the picture.
 * @param count The number of resource IDs in the array.
 * @note This structure is used to encapsulate the information needed to display a picture in the GUI.
 * @note The `ids` array contains the IDs of resources that are present at the specified coordinates.
 * @note The `count` field indicates how many resource IDs are in the `ids` array.
 */
typedef struct {
    int x;
    int y;
    int level;
    const int *ids;
    size_t count;
} gui_pic_t;

/**
 * @brief Structure representing an egg to be sent to the GUI.
 * @param egg_id The unique identifier of the egg.
 * @param player_id The ID of the player associated with the egg.
 */
typedef struct {
    int egg_id;
    int player_id;
    int x;
    int y;
} gui_enw_t;

/**
 * @brief Broadcasts the start of an incantation to all GUI clients.
 * @param net Pointer to the network structure containing the game state.
 * @param info Pointer to the gui_pic_t structure containing the picture information.
 * @note This function sends a notification about the start of an incantation to all connected GUI clients.
 * @note The `info` structure contains the coordinates, level, and resource IDs associated with the incantation.
 */
void gui_broadcast_pic(net_t *net, const gui_pic_t *info);
/**
 * @brief Broadcasts the egg laid by a player information to all GUI clients.
 * @param net Pointer to the network structure containing the game state.
 * @param info Pointer to the gui_enw_t structure containing the egg information.
 * @note This function sends a notification about an egg laid by a player to all connected GUI clients.
 * @note The `info` structure contains the egg ID, player ID, and coordinates of the egg.
 */
void gui_broadcast_enw(net_t *net, const gui_enw_t *info);
/**
 * @brief Broadcasts the player connection information to all GUI clients.
 * @param net Pointer to the network structure containing the game state.
 * @param egg_id The unique identifier of the egg that has hatched.
 * @note This function sends a notification about a player connection to all connected GUI clients.
 */
void gui_broadcast_ebo(net_t *net, int egg_id);
/**
 * @brief Broadcasts the death of an egg information to all GUI clients.
 * @param net Pointer to the network structure containing the game state.
 * @param egg_id The unique identifier of the egg that has finished incubation.
 * @note This function sends a notification about the death of an egg to all connected GUI clients.
 */
void gui_broadcast_edi(net_t *net, int egg_id);
/**
 * @brief Broadcasts the end of the game information to all GUI clients.
 * @param net Pointer to the network structure containing the game state.
 * @param team_name The name of the team to be broadcasted.
 * @note This function sends the end of the game to all connected GUI clients.
 */
void gui_broadcast_seg(net_t *net, const char *team_name);
/**
 * @brief Broadcasts a message from the server to all GUI clients.
 * @param net Pointer to the network structure containing the game state.
 * @param msg The message to be broadcasted.
 * @note This function signal a message send by the server to all connected GUI clients.
 */
void gui_broadcast_smg(net_t *net, const char *msg);

#endif /* GUI_H */
