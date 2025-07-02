/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin [WSL: Ubuntu]
** File description:
** Config
*/

#pragma once
#include <vector>

namespace Config {
    inline constexpr const char* WINDOW_TITLE = "Zappy";
    inline constexpr int TARGET_FPS = 120;

    inline constexpr bool ENABLE_VSYNC = true;
    inline constexpr bool ENABLE_TEXTURE_CACHING = true;
    inline constexpr bool ENABLE_PARTICLE_OPTIMIZATION = true;
    inline constexpr int MAX_PARTICLES = 1000;
    inline constexpr int TEXTURE_CACHE_SIZE = 1000;

    struct Resolution {
        int width;
        int height;
    };

    inline const std::vector<Resolution> SUPPORTED_RESOLUTIONS = {
        {640, 480},
        {800, 600},
        {1024, 768},
        {1280, 720},
        {1600, 900},
        {1920, 1080},
        {2560, 1440}
    };

    inline constexpr int DEFAULT_RESOLUTION_INDEX = 0;
}
