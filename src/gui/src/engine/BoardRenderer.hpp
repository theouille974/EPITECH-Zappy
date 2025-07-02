/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** BoardRenderer
*/

#pragma once

#include "../core/ServerUpdateManager.hpp"
#include "LayoutManager.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h> // For loading PNG images
#include <algorithm>
#include <cmath>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include "TextRenderer.hpp"

// Forward declarations to avoid circular dependencies
class ServerUpdateManager;

/**
 * @brief Renders the game board, including cells, resources, and eggs.
 */
class BoardRenderer {
    public:
        // --- Constructors and Destructor ---

        /**
         * @brief Constructs a BoardRenderer object with the given SDL_Renderer.
         * @param renderer The SDL_Renderer to use for rendering the board.
         * This constructor initializes the renderer and prepares it for drawing the
         * game board.
         */
        BoardRenderer(SDL_Renderer *renderer);

        /**
         * @brief Destructor that cleans up resources used by the BoardRenderer.
         * This destructor frees textures and other resources to prevent memory leaks.
         */
        ~BoardRenderer();

        // --- Board Configuration ---

        /**
         * @brief Sets the size of the game board.
         * @param width The width of the board in cells.
         * @param height The height of the board in cells.
         * This method updates the internal dimensions used for rendering the board.
         */
        void setBoardSize(int width, int height);

        /**
         * @brief Sets snoop mode and snoop resource textures.
         * @param snoopMode Whether to use snoop assets.
         * @param snoopTextures Array of snoop resource textures (7 textures).
         * This method allows switching between normal and snoop resource textures.
         */
        void setSnoopMode(bool snoopMode, SDL_Texture* snoopTextures[7]);

        // NEW: Set text renderer for drawing quantities
        void setTextRenderer(class TextRenderer* textRenderer);

        // --- Rendering Methods ---

        /**
         * @brief Renders the game board with the given layout metrics.
         * @param metrics The layout metrics containing dimensions and positions for
         * rendering.
         * @param getResources Function to retrieve resources for a specific cell.
         * @param getEggs Function to retrieve eggs for a specific cell.
         * @param selectedX The X coordinate of the selected cell.
         * @param selectedY The Y coordinate of the selected cell.
         */
        void render(
            const LayoutMetrics &metrics,
            std::function<const ServerUpdateManager::Resources *(int, int)>
                getResources,
            std::function<std::vector<const ServerUpdateManager::Egg *>(int, int)>
                getEggs,
            int selectedX, int selectedY);

        /**
         * @brief Renders the game board using legacy parameters.
         * @param viewportWidth The width of the viewport.
         * @param viewportHeight The height of the viewport.
         * @param getResources Function to retrieve resources for a specific cell.
         * @param getEggs Function to retrieve eggs for a specific cell.
         * @param selectedX The X coordinate of the selected cell.
         * @param selectedY The Y coordinate of the selected cell.
         */
        void render(
            int viewportWidth, int viewportHeight,
            std::function<const ServerUpdateManager::Resources *(int, int)>
                getResources,
            std::function<std::vector<const ServerUpdateManager::Egg *>(int, int)>
                getEggs,
            int selectedX, int selectedY);

    private:
        // --- Private Members ---

        SDL_Renderer *_renderer;
        int _boardWidth{0};
        int _boardHeight{0};

        // Textures
        SDL_Texture *_eggTexture{nullptr};
        std::unordered_map<std::string, SDL_Texture *> _teamEggTextures;

        // Caching
        std::string _lastTeamColorMod = "";

        // Rendering Options
        bool _showGrid = true;
        bool _showCellBorders = true;
        bool _showResourceIcons = true;
        float _cellPadding = 0.1f; // 10% padding within cells

        // NEW: Resource textures
        bool loadResourceSprites();
        SDL_Texture* _resourceTextures[7] {nullptr}; // 0 food, 1-6 gems

        // SNOOP_DOG Easter Egg
        bool _snoopMode = false;
        SDL_Texture* _snoopResourceTextures[7] {nullptr}; // Snoop resource textures

        // NEW: Pointer to text renderer for quantity annotations
        TextRenderer* _textRenderer{nullptr};

        // --- Helper Methods (Internal Logic) ---

        /**
         * @brief Loads the egg sprite texture from the specified file.
         * @return True if the texture was loaded successfully, false otherwise.
         * This method initializes the egg texture used for rendering eggs on the
         * board.
         */
        bool loadEggSprite();

        /**
         * @brief Draws an egg at the specified position with the given dimensions.
         * @param x The X coordinate of the egg's position.
         * @param y The Y coordinate of the egg's position.
         * @param cellW The width of the cell in which the egg is drawn.
         * @param cellH The height of the cell in which the egg is drawn.
         * @param team The team color associated with the egg.
         * @param isSelected Whether the egg is selected (for highlighting).
         */
        void drawEgg(int x, int y, float cellW, float cellH, const std::string &team,
                    bool isSelected = false);

        /**
         * @brief Gets the texture for the specified team egg.
         * @param team The team name to get the egg texture for.
         * @return The SDL_Texture for the team's egg, or nullptr if not found.
         * This method retrieves a cached texture for the specified team, loading it
         * if necessary.
         */
        SDL_Texture *getTeamEggTexture(const std::string &team);

        /**
         * @brief Sets the team color modifier for rendering.
         * @param team The team name to set the color modifier for.
         * This method updates the last used team color to avoid redundant operations.
         */
        void setTeamColorMod(const std::string &team);

        /**
         * @brief Draws the background of a cell.
         * @param x The X coordinate of the cell.
         * @param y The Y coordinate of the cell.
         * @param cellW The width of the cell.
         * @param cellH The height of the cell.
         * @param isSelected Whether the cell is selected (for highlighting).
         */
        void drawCellBackground(int x, int y, float cellW, float cellH,
                                bool isSelected);

        /**
         * @brief Draws the border of a cell.
         * @param x The X coordinate of the cell.
         * @param y The Y coordinate of the cell.
         * @param cellW The width of the cell.
         * @param cellH The height of the cell.
         */
        void drawCellBorder(int x, int y, float cellW, float cellH);

        /**
         * @brief Draws the resource icons in a cell.
         * @param x The X coordinate of the cell.
         * @param y The Y coordinate of the cell.
         * @param cellW The width of the cell.
         * @param cellH The height of the cell.
         * @param resources A pointer to the resources for the cell.
         */
        void drawResources(int x, int y, float cellW, float cellH,
                            const ServerUpdateManager::Resources *resources);

        /**
         * @brief Draws a single resource icon.
         * @param x The X coordinate of the icon.
         * @param y The Y coordinate of the icon.
         * @param size The size of the resource icon.
         * @param resourceType The type of resource to draw.
         * @param quantity The quantity of the resource to display.
         */
        void drawResourceIcon(int x, int y, int size, int resourceType, int quantity);

        /**
         * @brief Gets the dimensions of a cell with padding applied.
         * @param x The X coordinate of the cell.
         * @param y The Y coordinate of the cell.
         * @param cellW The unpadded width of the cell.
         * @param cellH The unpadded height of the cell.
         * @return An SDL_Rect containing the padded position and dimensions of the
         * cell.
         */
        SDL_Rect getCellRect(int x, int y, float cellW, float cellH);
};
