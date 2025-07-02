/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** Theme
*/

#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <unordered_map>

namespace GUI {

    // --- Theme Structure ---

    /**
     * @brief Represents a theme for the GUI, containing colors for various UI
     * elements.
     */
    struct Theme {
    // General UI Colors
    SDL_Color windowBackground;
    SDL_Color text;
    SDL_Color textDisabled;
    SDL_Color textHighlight;
    SDL_Color accent;

    // Sidebar Colors
    SDL_Color sidebarBg;
    SDL_Color sidebarBorder;

    // Panel Colors (used within sidebar)
    SDL_Color panelBg;
    SDL_Color panelBorder;

    // Overlay Panel Colors (player stats)
    SDL_Color overlayBg;
    SDL_Color overlayBorder;
    SDL_Color overlayAccent;

    // Team Colors
    std::unordered_map<std::string, SDL_Color> teamColors;
    };

    // --- Theme Definitions ---

    /**
     * @brief Returns the default GameCube-inspired theme.
     * This theme features a dark purple background with bright accents.
     * It is designed to be visually appealing and functional for the GUI.
     */
    inline Theme getGameCubeTheme() {
    Theme theme;

    // General
    theme.windowBackground = {20, 0, 40, 255};
    theme.text = {220, 220, 240, 255};
    theme.textDisabled = {100, 90, 110, 255};
    theme.textHighlight = {255, 255, 100, 255};
    theme.accent = {180, 0, 255, 255};

    // Sidebar (more transparent)
    theme.sidebarBg = {25, 0, 40, 10}; // Lowered alpha from 150 to 80
    theme.sidebarBorder = {140, 0, 200, 255};

    // Panels (more transparent)
    theme.panelBg = {45, 0, 70, 10}; // Lowered alpha from 170 to 100
    theme.panelBorder = {90, 0, 130, 255};

    // Overlay (solid purple for player monitor)
    theme.overlayBg = {65, 20, 90, 255};      // Fully opaque
    theme.overlayBorder = {180, 50, 220, 255}; // Brighter border
    theme.overlayAccent = {255, 255, 255, 255};

    // Team Colors
    theme.teamColors["RED"] = {255, 80, 80, 255};
    theme.teamColors["BLUE"] = {100, 140, 255, 255};
    theme.teamColors["GREEN"] = {80, 240, 150, 255};
    theme.teamColors["YELLOW"] = {255, 240, 80, 255};
    theme.teamColors["SNOOP_DOG"] = {80, 240, 150, 255};
    theme.teamColors["DEFAULT"] = {200, 200, 200, 255};

    return theme;
    }

    /**
     * @brief Returns a pastel pink theme inspired by GameBoy Advance.
     * This theme features a soft pink background with hot pink accents.
     */
    inline Theme getGameboyPinkTheme() {
    Theme theme = getGameCubeTheme();
    // Pastel pink inspired by GameBoy Advance
    theme.windowBackground = {210, 120, 160, 255};
    theme.sidebarBg = {180, 80, 120, 170};
    theme.sidebarBorder = {255, 105, 180, 255}; // Hot pink border
    theme.panelBg = {200, 100, 140, 180};
    theme.panelBorder = {150, 60, 100, 255};
    theme.overlayBg = {160, 80, 110, 230};
    theme.overlayBorder = {255, 105, 180, 255};
    theme.accent = {255, 105, 180, 255};
    return theme;
    }

    /**
     * @brief Returns a Matrix-themed color scheme with green text on a black
     * background. This theme is inspired by the iconic Matrix movie aesthetic.
     */
    inline Theme getMatrixTheme() {
    Theme theme = getGameCubeTheme();
    // Matrix green on black
    theme.windowBackground = {5, 5, 5, 255};
    theme.text = {0, 255, 70, 255};
    theme.textHighlight = {180, 255, 180, 255};
    theme.accent = {0, 255, 70, 255};
    theme.sidebarBg = {0, 20, 0, 180};
    theme.sidebarBorder = {0, 255, 70, 255};
    theme.panelBg = {0, 30, 0, 180};
    theme.panelBorder = {0, 200, 50, 255};
    theme.overlayBg = {0, 40, 0, 255};
    theme.overlayBorder = {0, 255, 70, 255};
    return theme;
    }

    /**
     * @brief Returns a Nintendo 64-inspired theme with navy blue and yellow
     * accents. This theme captures the nostalgic colors of the N64 console.
     */
    inline Theme getN64Theme() {
    Theme theme = getGameCubeTheme();
    // Navy blue & yellow similar to Nintendo 64 logo
    theme.windowBackground = {0, 0, 40, 255};
    theme.text = {230, 230, 255, 255};
    theme.accent = {255, 200, 0, 255};
    theme.sidebarBg = {0, 0, 60, 150};
    theme.sidebarBorder = {0, 0, 120, 255};
    theme.panelBg = {0, 0, 80, 160};
    theme.panelBorder = {255, 200, 0, 255};
    theme.overlayBg = {0, 0, 90, 255};
    theme.overlayBorder = {255, 200, 0, 255};
    return theme;
    }

    /**
     * @brief Returns a red-themed color scheme with dark red backgrounds and bright
     * red accents. This theme is designed to be bold and visually striking.
     */
    inline Theme getRedTheme() {
    Theme theme;
    theme.windowBackground = {40, 0, 0, 255};
    theme.text = {255, 220, 220, 255};
    theme.textDisabled = {120, 80, 80, 255};
    theme.textHighlight = {255, 255, 200, 255};
    theme.accent = {255, 60, 60, 255};

    theme.sidebarBg = {60, 0, 0, 180};
    theme.sidebarBorder = {255, 60, 60, 255};

    theme.panelBg = {80, 0, 0, 180};
    theme.panelBorder = {200, 50, 50, 255};

    theme.overlayBg = {90, 10, 10, 255};
    theme.overlayBorder = {255, 60, 60, 255};

    theme.teamColors["DEFAULT"] = {255, 60, 60, 255};
    return theme;
    }

    /**
     * @brief Returns a white theme with translucent elements.
     * This theme is designed to be clean and neutral, suitable for light-themed
     * applications.
     */
    inline Theme getWhiteTheme() {
    Theme theme;
    theme.windowBackground = {20, 20, 30, 255}; // neutral background so no tint
    theme.text = {240, 240, 240, 255};
    theme.textDisabled = {180, 180, 180, 255};
    theme.textHighlight = {255, 255, 255, 255};
    theme.accent = {200, 200, 200, 255};

    theme.sidebarBg = {255, 255, 255, 100}; // translucent white
    theme.sidebarBorder = {220, 220, 220, 255};

    theme.panelBg = {255, 255, 255, 90};
    theme.panelBorder = {200, 200, 200, 255};

    theme.overlayBg = {255, 255, 255, 120};
    theme.overlayBorder = {240, 240, 240, 255};

    theme.teamColors["DEFAULT"] = {200, 200, 200, 255};
    return theme;
    }

    // --- Static Theme Manager ---

    /**
     * @brief Manages the current theme for the GUI.
     */
    class ThemeManager {
        public:
        /**
         * @brief Loads a new theme into the GUI.
         * @param theme The Theme object to load.
         */
        static void loadTheme(const Theme &theme) { _currentTheme = theme; }

        /**
         * @brief Returns the currently loaded theme.
         * @return The current Theme object.
         */
        static const Theme &getTheme() { return _currentTheme; }

    private:
        inline static Theme _currentTheme =
            getGameCubeTheme(); // Default to GCN theme

    };
} // namespace GUI
