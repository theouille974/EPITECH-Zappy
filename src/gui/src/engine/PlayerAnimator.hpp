/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** PlayerAnimator
*/

#pragma once
#include <SDL2/SDL.h>
#include <unordered_map>
#include <string>
#include "TweenSystem.hpp"
#include "../core/ServerUpdateManager.hpp"
#include "TextRenderer.hpp"

class PlayerAnimator {
    public:
        /**
         * @brief Constructs a PlayerAnimator object.
         * @param renderer The SDL_Renderer to use for rendering players.
         * @param tween The TweenSystem to manage animations.
         */
        PlayerAnimator(SDL_Renderer* renderer, TweenSystem& tween);

        /**
         * @brief Destructor that cleans up resources used by PlayerAnimator.
         * @note This destructor frees textures and other resources to prevent memory leaks.
         */
        ~PlayerAnimator();

        /**
         * @brief Synchronizes the internal player visuals with the server state.
         * @param server The ServerUpdateManager containing the latest player states.
         * @note This method updates the internal visuals based on the server's player data,
         * @note removing any players that are no longer present and updating the positions,
         * @note orientations, levels, teams, and life percentages of existing players.
         */
        void syncWithServer(const ServerUpdateManager& server);

        /**
         * @brief Updates the tween system with the elapsed time.
         * @param dt The delta time in seconds since the last update.
         * @note This method advances all active tweens by the specified time delta.
         */
        void update(float dt) { _tween.update(dt); }

        /**
         * @brief Renders all players on the screen.
         * @param viewportWidth The width of the viewport in pixels.
         * @param viewportHeight The height of the viewport in pixels.
         * @param boardW The width of the game board in cells.
         * @param boardH The height of the game board in cells.
         * @param selectedPlayerId The ID of the currently selected player (for highlighting).
         * @param text The TextRenderer used for drawing player names and life meters.
         * @note This method draws all players on the screen, including their life meters and
         * @note team colors. It uses the TweenSystem to animate player movements smoothly.
         */
        void render(int viewportWidth, int viewportHeight,
                    int boardW, int boardH,
                    int selectedPlayerId,
                    TextRenderer* text);

    private:
        struct Visual {
            float x; // tile coordinate (can be fractional during animation)
            float y;
            int   orientation{1};
            int   level{1};
            std::string team;
            float lifePercentage{1.f};
        };

        SDL_Renderer* _renderer;
        TweenSystem&  _tween;
        std::unordered_map<int, Visual> _visuals;

        // Special texture for SNOOP_DOG team
        SDL_Texture* _snoopTexture = nullptr;

        /**
         * @brief Draws a player pixel on the screen.
         * @param px The X coordinate of the player in pixels.
         * @param py The Y coordinate of the player in pixels.
         * @param radius The radius of the player pixel.
         * @param color The color of the player pixel.
         * @param lifePercentage The percentage of life remaining (0.0 to 1.0).
         * @param playerId The unique identifier for the player.
         * @param orientation The orientation of the player (1=North, 2=East, 3=South, 4=West).
         * @param text The TextRenderer used for drawing player names and life meters.
         * @param highlight Whether to highlight the player (e.g., for selection).
         * @param team The team name of the player (for team color).
         * @note This method draws a pixel representation of the player, including their life
         * @note meter and orientation indicator. It uses the specified color and applies
         * @note highlighting if requested. The player pixel is drawn at the specified
         * @note coordinates with the given radius.
         */
        void drawPlayerPixel(float px, float py, int radius, const SDL_Color& color,
                            float lifePercentage, int playerId, int orientation, TextRenderer* text,
                            bool highlight, const std::string& team);
};
