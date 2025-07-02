/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** LayoutManager
*/

#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <functional>

/**
 * @brief Struct to hold layout metrics for the game UI.
 * This struct contains all necessary dimensions and positions for rendering the
 * game board, sidebar, and other UI elements. It is used to ensure consistent
 * layout across different screen sizes and resolutions.
 */
struct LayoutMetrics {
    // Window dimensions
    int windowWidth;
    int windowHeight;

    // Board area
    int boardAreaX;
    int boardAreaY;
    int boardAreaWidth;
    int boardAreaHeight;

    // Sidebar area
    int sidebarX;
    int sidebarY;
    int sidebarWidth;
    int sidebarHeight;

    // Cell dimensions for board
    float cellWidth;
    float cellHeight;

    // UI element spacing
    int padding;
    int margin;
    int buttonHeight;
    int textLineHeight;

    // Responsive breakpoints
    bool isCompactMode;
    bool isWideMode;
};

class LayoutManager {
    public:
        /**
         * @brief Constructs a LayoutManager object.
         * This constructor initializes the layout manager with default settings and
         */
        LayoutManager();

        /**
         * @brief Calculates layout metrics based on window and board dimensions.
         * @param windowWidth The width of the window in pixels.
         * @param windowHeight The height of the window in pixels.
         * @param boardWidth The width of the game board in cells.
         * @param boardHeight The height of the game board in cells.
         * @return LayoutMetrics containing dimensions and positions for rendering.
         */
        LayoutMetrics calculateLayout(int windowWidth, int windowHeight, int boardWidth, int boardHeight);

        /**
         * @brief Gets the sidebar width based on the current window width.
         * @param windowWidth The width of the window in pixels.
         * @return The calculated sidebar width in pixels, which adapts to the window size.
         */
        int getSidebarWidth(int windowWidth);

        /**
         * @brief Gets the minimum window size required for the game.
         * @return SDL_Point containing the minimum width and height for the window.
         */
        SDL_Point getMinimumWindowSize();

        /**
         * @brief Checks if the layout should switch to compact mode based on window size.
         * @param windowWidth The width of the window in pixels.
         * @param windowHeight The height of the window in pixels.
         * @return True if compact layout should be used, false otherwise.
         */
        bool shouldUseCompactLayout(int windowWidth, int windowHeight);

        /**
         * @brief Gets the UI scale factor based on the window size.
         * @param windowWidth The width of the window in pixels.
         * @param windowHeight The height of the window in pixels.
         * @return The scale factor for UI elements, which adapts to the window size.
         */
        float getUIScale(int windowWidth, int windowHeight);

        /**
         * @brief Converts board coordinates to screen coordinates.
         * @param boardX The X coordinate on the game board.
         * @param boardY The Y coordinate on the game board.
         * @param metrics The layout metrics containing dimensions and positions for rendering.
         * @return SDL_Point containing the screen coordinates corresponding to the board position.
         */
        SDL_Point boardToScreen(int boardX, int boardY, const LayoutMetrics& metrics);

        /**
         * @brief Converts screen coordinates to board coordinates.
         * @param screenX The X coordinate on the screen.
         * @param screenY The Y coordinate on the screen.
         * @param metrics The layout metrics containing dimensions and positions for rendering.
         * @return SDL_Point containing the board coordinates corresponding to the screen position.
         */
        SDL_Point screenToBoard(int screenX, int screenY, const LayoutMetrics& metrics);

        /**
         * @brief Checks if a point is within the board area.
         * @param screenX The X coordinate on the screen.
         * @param screenY The Y coordinate on the screen.
         * @param metrics The layout metrics containing dimensions and positions for rendering.
         * @return True if the point is within the board area, false otherwise.
         */
        bool isPointInBoardArea(int screenX, int screenY, const LayoutMetrics& metrics);

        /**
         * @brief Checks if a point is within the sidebar area.
         * @param screenX The X coordinate on the screen.
         * @param screenY The Y coordinate on the screen.
         * @param metrics The layout metrics containing dimensions and positions for rendering.
         * @return True if the point is within the sidebar area, false otherwise.
         */
        bool isPointInSidebarArea(int screenX, int screenY, const LayoutMetrics& metrics);

    private:
        static constexpr int MIN_SIDEBAR_WIDTH = 180;
        static constexpr int PREFERRED_SIDEBAR_WIDTH = 250;
        static constexpr int MAX_SIDEBAR_WIDTH = 400;
        static constexpr int MIN_WINDOW_WIDTH = 800;
        static constexpr int MIN_WINDOW_HEIGHT = 600;
        static constexpr int COMPACT_BREAKPOINT = 1000;
        static constexpr int WIDE_BREAKPOINT = 1600;
};
