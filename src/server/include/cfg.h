/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** cfg
*/

#include <stdbool.h>

#ifndef CFG_H
    #define CFG_H

/**
 * @brief Configuration structure for the server.
 */
typedef struct s_cfg {
    int port;
    int width;
    int height;
    char **teams;
    int team_count;
    int clients_nb;
    int freq;
} cfg_t;

/**
 * @brief Initializes the configuration structure with default values.
 * @param cfg Pointer to the configuration structure to initialize.
 * @param ac Argument count.
 * @param av Argument vector.
 */
bool cfg_parse(cfg_t *cfg, int ac, char **av);

/**
 * @brief Sets default values for the configuration structure.
 * @param cfg Pointer to the configuration structure to set defaults for.
 */
void cfg_free(cfg_t *cfg);

#endif /* CFG_H */
