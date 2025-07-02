/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** SidebarRenderer
*/

#pragma once
#include <SDL2/SDL.h>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include "LayoutManager.hpp"
#include "../core/ServerUpdateManager.hpp"
#include "../core/Theme.hpp"

class TextRenderer;

class SidebarRenderer {
    public:
        // Constructors and Destructors
        /**
         * @brief Constructs a SidebarRenderer with the given SDL_Renderer and TextRenderer.
         * @param renderer The SDL_Renderer used for rendering.
         * @param textRenderer The TextRenderer used for rendering text.
         */
        SidebarRenderer(SDL_Renderer* renderer, TextRenderer* textRenderer);
        /**
         * @brief Destructor for SidebarRenderer.
         * Cleans up any allocated resources.
         */
        ~SidebarRenderer();

        /**
         * @brief Renders the sidebar with the current game state.
         * @param metrics The layout metrics for positioning elements.
         * @param server The server update manager containing game data.
         * @param selectedX The X coordinate of the selected tile.
         * @param selectedY The Y coordinate of the selected tile.
         * @param selectedPlayerId The ID of the currently selected player.
         * @param clickablePlayerRects A vector of clickable player rectangles for interaction.
         */
        void render(const LayoutMetrics& metrics,
                    const ServerUpdateManager& server,
                    int selectedX, int selectedY,
                    int selectedPlayerId,
                    const std::vector<std::pair<SDL_Rect, int>>& clickablePlayerRects);

        /**
         * @brief Sets the game start time.
         * @param startTime The start time in milliseconds since the epoch.
         * @note This is used to calculate the elapsed game time.
         */
        void setGameStartTime(Uint32 startTime);

        /**
         * @brief Updates the clickable areas for player rectangles.
         * @param metrics The layout metrics for positioning elements.
         * @param server The server update manager containing game data.
         * @param clickablePlayerRects A vector to store the updated clickable player rectangles.
         * @note This function calculates the clickable areas based on player positions and the layout metrics.
         * @note The clickable areas are used for player selection and interaction.
         */
        void updateClickableAreas(const LayoutMetrics& metrics,
                                const ServerUpdateManager& server,
                                std::vector<std::pair<SDL_Rect, int>>& clickablePlayerRects);

        /**
         * @brief Gets the player ID at a specific screen position.
         * @param screenX The X coordinate on the screen.
         * @param screenY The Y coordinate on the screen.
         * @param metrics The layout metrics for positioning elements.
         * @param clickablePlayerRects A vector of clickable player rectangles.
         * @return The player ID at the specified position, or -1 if no player is found.
         * @note This function checks if the given screen coordinates fall within any clickable player rectangle.
         * @note If a player is found, their ID is returned; otherwise, -1 is returned.
         */
        int getPlayerIdAt(int screenX, int screenY, const LayoutMetrics& metrics,
                        const std::vector<std::pair<SDL_Rect, int>>& clickablePlayerRects);

        /**
         * @brief Checks if the timer area was clicked.
         * @param screenX The X coordinate on the screen.
         * @param screenY The Y coordinate on the screen.
         * @return True if the timer area was clicked, false otherwise.
         * @note If the timer area is clicked, it toggles the timer mode.
         */
        bool isTimerClicked(int screenX, int screenY) const;
        /**
         * @brief Toggles the timer mode.
         * @note This function switches between showing and hiding the game timer.
         */
        void toggleTimerMode();
        /**
         * @brief Scrolls the sidebar by a specified amount.
         * @param amount The amount to scroll (positive or negative).
         * @note This function adjusts the scroll offset of the sidebar.
         */
        void scroll(int amount);

        /**
         * @brief Loads resource sprites for the sidebar.
         * @return True if the sprites were loaded successfully, false otherwise.
         * @note This function loads the textures for the resources used in the sidebar.
         * @note It initializes the resource textures array with the loaded textures.
         */
        bool loadResourceSprites();

        /**
         * @brief Sets the snoop mode for the sidebar.
         * @param snoopMode True to enable snoop mode, false to disable it.
         * @param snoopTextures An array of textures for snoop mode resources.
         * @note In snoop mode, the sidebar displays resource information for all players.
         * @note The snoopTextures array contains textures for the snooped resources.
         */
        void setSnoopMode(bool snoopMode, SDL_Texture* snoopTextures[7]);

    private:
        SDL_Renderer* _renderer;
        TextRenderer* _textRenderer;

        Uint32 _gameStartTime = 0;
        bool _showGameTime = false;
        SDL_Rect _timerRect;

        int _scrollOffset = 0;

        bool _snoopMode = false;
        SDL_Texture* _snoopResourceTextures[7] {nullptr}; // Snoop resource textures
        SDL_Texture* _resourceTextures[7] {nullptr}; // index 0=food, 1-6 gems

        const SDL_Color _bgDark{25, 0, 40, 180};
        const SDL_Color _bgMedium{45, 0, 70, 200};
        const SDL_Color _bgLight{65, 0, 90, 220};
        const SDL_Color _borderBright{140, 0, 200, 255};
        const SDL_Color _borderDim{90, 0, 130, 255};

        /**
         * @brief Renders the game timer in the sidebar.
         * @param metrics The layout metrics for positioning elements.
         * @param server The server update manager containing game data.
         */
        void renderGameTimer(const LayoutMetrics& metrics, const ServerUpdateManager& server);
        /**
         * @brief Renders the player list in the sidebar.
         * @param metrics The layout metrics for positioning elements.
         * @param server The server update manager containing game data.
         * @param clickablePlayerRects A vector of clickable player rectangles for interaction.
         * @note This function displays the list of players with their IDs, levels, and life meters.
         */
        void renderPlayerList(const LayoutMetrics& metrics, const ServerUpdateManager& server,
                            std::vector<std::pair<SDL_Rect, int>>& clickablePlayerRects);
        /**
         * @brief Renders the resource legend in the sidebar.
         * @param metrics The layout metrics for positioning elements.
         * @param server The server update manager containing game data.
         * @note This function displays the available resources and their quantities.
         * @note It shows the resources for the selected player or all players in snoop mode
         */
        void renderResourceLegend(const LayoutMetrics& metrics, const ServerUpdateManager& server);
        /**
         * @brief Renders information about the selected tile in the sidebar.
         * @param metrics The layout metrics for positioning elements.
         * @param server The server update manager containing game data.
         * @param selectedX The X coordinate of the selected tile.
         * @param selectedY The Y coordinate of the selected tile.
         * @note This function displays the resources available on the selected tile.
         */
        void renderSelectedTileInfo(const LayoutMetrics& metrics, const ServerUpdateManager& server,
                                int selectedX, int selectedY, int selectedPlayerId);
        /**
         * @brief Renders information about the selected player in the sidebar.
         * @param metrics The layout metrics for positioning elements.
         * @param server The server update manager containing game data.
         * @param selectedPlayerId The ID of the currently selected player.
         * @note This function displays the selected player's ID, level, team, life meter,
         * @note and resources.
         */
        void renderSelectedPlayerInfo(const LayoutMetrics& metrics, const ServerUpdateManager& server,
                                    int selectedPlayerId);

        /**
         * @brief Draws a panel with a background color and border.
         * @param rect The rectangle defining the panel area.
         * @param bgColor The background color of the panel.
         * @param borderColor The border color of the panel.
         * @note This function draws a filled rectangle for the background and a border around it.
         * @note It is used to create visually distinct sections in the sidebar.
         */
        void drawPanel(const SDL_Rect& rect, const SDL_Color& bgColor, const SDL_Color& borderColor);
        /**
         * @brief Draws a progress bar with a filled color and background color.
         * @param x The X coordinate of the progress bar.
         * @param y The Y coordinate of the progress bar.
         * @param width The width of the progress bar.
         * @param height The height of the progress bar.
         * @param percentage The fill percentage (0.0 to 1.0).
         * @param fillColor The color used to fill the progress bar.
         * @param bgColor The background color of the progress bar.
         * @note This function draws a filled rectangle representing the progress and a background rectangle.
         */
        void drawProgressBar(int x, int y, int width, int height, float percentage,
                            const SDL_Color& fillColor, const SDL_Color& bgColor);
        /**
         * @brief Draws a resource icon at the specified position.
         * @param x The X coordinate of the icon.
         * @param y The Y coordinate of the icon.
         * @param size The size of the icon (width and height).
         * @param resourceType The type of resource (0 for food, 1-6 for gems).
         * @note This function draws a resource icon based on the resource type.
         * @note It uses the resource textures loaded in the constructor.
         */
        void drawResourceIcon(int x, int y, int size, int resourceType);

        /**
         * @brief Draws a panel without a bottom border.
         * @param rect The rectangle defining the panel area.
         * @param bgColor The background color of the panel.
         * @param borderColor The border color of the panel.
         * @note This function draws a filled rectangle for the background and a border around it,
         * but does not draw the bottom border.
         */
        void drawPanelNoBottom(const SDL_Rect& rect, const SDL_Color& bgColor, const SDL_Color& borderColor);

        /**
         * @brief Calculates the Y position for the player list based on layout metrics.
         * @param metrics The layout metrics for positioning elements.
         * @return The Y coordinate for the player list.
         * @note This function computes the vertical position of the player list in the sidebar.
         */
        int calculatePlayerListY(const LayoutMetrics& metrics);
        /**
         * @brief Calculates the Y position for the resource legend based on layout metrics.
         * @param metrics The layout metrics for positioning elements.
         * @param server The server update manager containing game data.
         * @return The Y coordinate for the resource legend.
         * @note This function computes the vertical position of the resource legend in the sidebar.
         */
        int calculateResourceLegendY(const LayoutMetrics& metrics, const ServerUpdateManager& server);
        /**
         * @brief Calculates the Y position for the selected tile info based on layout metrics.
         * @param metrics The layout metrics for positioning elements.
         * @return The Y coordinate for the selected tile info.
         * @note This function computes the vertical position of the selected tile info in the sidebar.
         */
        int calculateSelectedTileY(const LayoutMetrics& metrics, const ServerUpdateManager& server);

        /**
         * @brief Computes the rectangle for the player panel based on layout metrics and selected player ID.
         * @param metrics The layout metrics for positioning elements.
         * @param server The server update manager containing game data.
         * @param selectedPlayerId The ID of the currently selected player.
         * @return The SDL_Rect representing the player panel area.
         * @note This function calculates the position and size of the player panel in the sidebar.
         */
        SDL_Rect computePlayerPanelRect(const LayoutMetrics& metrics,
                                        const ServerUpdateManager& server,
                                        int selectedPlayerId);
};
