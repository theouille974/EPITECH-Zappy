/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** team
*/

#include "team.h"
#include <string.h>

void teams_init(team_t *arr, int count, char **names, int slots)
{
    for (int i = 0; i < count; ++i) {
        arr[i].name = names[i];
        arr[i].slots = slots;
    }
}

int team_find(team_t *arr, int count, const char *name)
{
    for (int i = 0; i < count; ++i)
        if (!strcmp(arr[i].name, name))
            return i;
    return -1;
}

bool team_take_slot(team_t *arr, int count, const char *name, int *remaining)
{
    int idx = team_find(arr, count, name);

    if (idx < 0)
        return false;
    if (arr[idx].slots <= 0)
        return false;
    --arr[idx].slots;
    if (remaining)
        *remaining = arr[idx].slots;
    return true;
}

void team_release_slot(team_t *arr, int idx)
{
    if (idx >= 0)
        ++arr[idx].slots;
}
