/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin [WSL: Ubuntu]
** File description:
** App
*/
#pragma once

#include "../engine/AudioManager.hpp"
#include "../engine/Effects.hpp"
#include "../engine/LayoutManager.hpp"
#include "../engine/PlayerAnimator.hpp"
#include "../engine/SidebarRenderer.hpp"
#include "../engine/TweenSystem.hpp"
#include "BoardRenderer.hpp"
#include "Config.hpp"
#include "ServerUpdateManager.hpp"
#include "TextRenderer.hpp"
#include <SDL2/SDL.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class App {
    public:
        // --- Constructors and Destructor ---

        /**
         * Constructor that initializes the application with a specific server hostname
         * and port.
         */
        App(const std::string &hostname, int port);

        /**
         * Default constructor that initializes the application with default server
         * settings.
         */
        App();

        /**
         * Destructor that cleans up resources.
         */
        ~App();

        /**
         * Main application loop that runs the game.
         */
        void run();

    private:
        // --- Initialization and Core Loop Methods ---

        /**
         * Initializes the application, setting up SDL, creating the window and
         * renderer, and loading necessary resources.
         */
        void initializeApp();

        /**
         * Starts the game by connecting to the server and initializing game state.
         * @return True if the game started successfully, false otherwise.
         */
        bool startGame();

        /**
         * Updates the application state based on the time delta.
         * @param dt Time delta since the last update in seconds.
         */
        void update(float dt);

        /**
         * Handles events such as keyboard and mouse input.
         * @param running Reference to a boolean that indicates if the application
         * should continue running.
         */
        void handleEvents(bool &running);

        /**
         * Renders the current state of the application.
         */
        void render();

        // --- Rendering Helper Methods ---

        /**
         * Renders the help screen based on the current help page.
         */
        void renderHelpScreen();

        /**
         * Draws a rounded rectangle at the specified position with the given
         * dimensions and radius.
         * @param x X coordinate of the rectangle's top-left corner.
         * @param y Y coordinate of the rectangle's top-left corner.
         * @param w Width of the rectangle.
         * @param h Height of the rectangle.
         * @param radius Radius of the corners.
         * @param r Red component of the color.
         * @param g Green component of the color.
         * @param b Blue component of the color.
         * @param a Alpha component of the color.
         */
        void drawRoundedRect(int x, int y, int w, int h, int radius, Uint8 r,
                            Uint8 g, Uint8 b, Uint8 a);

        /**
         * Draws a filled circle at the specified position with the given radius.
         * @param centerX X coordinate of the circle's center.
         * @param centerY Y coordinate of the circle's center.
         * @param radius Radius of the circle.
         */
        void drawFilledCircle(int centerX, int centerY, int radius);

        /**
         * Draws a life bar at the specified position with the given dimensions and
         * life percentage.
         * @param x X coordinate of the life bar's top-left corner.
         * @param y Y coordinate of the life bar's top-left corner.
         * @param width Width of the life bar.
         * @param height Height of the life bar.
         * @param lifePercentage Percentage of life remaining (0.0 to 1.0).
         */
        void drawLifeBar(int x, int y, int width, int height, float lifePercentage);

        /**
         * Draws a player at the specified position with the given radius, color, life
         * percentage, and orientation.
         * @param x X coordinate of the player's center.
         * @param y Y coordinate of the player's center.
         * @param radius Radius of the player circle.
         * @param color Color of the player.
         * @param lifePercentage Percentage of life remaining (0.0 to 1.0).
         * @param isMultiple Whether this player is part of a group (for visual
         * effects).
         * @param playerId Unique identifier for the player.
         * @param orientation Orientation of the player (1=North, 2=East, 3=South,
         * 4=West).
         */
        void drawPlayer(int x, int y, int radius, const SDL_Color &color,
                        float lifePercentage, bool isMultiple, int playerId,
                        int orientation);

        /**
         * Draws a direction indicator for the player based on their orientation.
         * @param x X coordinate of the player's center.
         * @param y Y coordinate of the player's center.
         * @param radius Radius of the player circle.
         * @param orientation Orientation of the player (1=North, 2=East, 3=South,
         * 4=West).
         */
        void drawDirectionIndicator(int x, int y, int radius, int orientation);

        /**
         * Draws the parallax background based on the current offsets.
         * This creates a scrolling effect for the background.
         * The background speed can be adjusted with bgSpeedX and bgSpeedY.
         */
        void drawParallaxBackground();

        /**
         * Draws a help button at the specified position with the given dimensions.
         * @param x X coordinate of the button's top-left corner.
         * @param y Y coordinate of the button's top-left corner.
         * @param w Width of the button.
         * @param h Height of the button.
         * @param hovering Whether the mouse is hovering over the button.
         */
        void drawHelpButton(int x, int y, int w, int h, bool hovering);

        /**
         * Loads SNOOP_DOG easter egg assets (background and resources).
         * @return true if all assets loaded successfully, false otherwise.
         */
        bool loadSnoopAssets();

        // --- Configuration and State Management ---

        /**
         * Applies the selected resolution to the window.
         * @param index Index of the resolution in the supported resolutions list.
         */
        void applyResolution(int index);

        /**
         * Toggles between different resolutions.
         */
        void toggleResolution();

        /**
         * Method to update time unit from server.
         * @param f The new time unit value.
         */
        void setTimeUnit(int f) { timeUnit = f; }

        // --- Private Members ---

        // SDL related
        SDL_Window *window = nullptr;
        SDL_Renderer *renderer = nullptr;
        SDL_Surface *cursorSurface = nullptr;
        SDL_Texture *cursorTexture = nullptr;

        // Audio
        std::unique_ptr<AudioManager> audio;

        // Server and Game Logic
        std::string serverHostname;
        int serverPort;
        ServerUpdateManager server;

        // Host / port input fields (menu)
        std::string inputHostname;
        std::string inputPort;
        bool editingHostname = false;
        bool editingPort = false;

        std::unique_ptr<BoardRenderer> board;
        std::unique_ptr<TextRenderer> text;
        std::unique_ptr<PlayerAnimator> playerAnimator;
        std::unique_ptr<EffectSystem> effectSystem;
        std::unique_ptr<SidebarRenderer> sidebarRenderer;

        // Game State Variables
        Uint32 lastFrameTime = 0;
        bool fullscreen = false;
        int resolutionIndex = Config::DEFAULT_RESOLUTION_INDEX;
        int selectedX = -1;
        int selectedY = -1;
        int selectedPlayerId = -1;
        std::vector<std::pair<SDL_Rect, int>> clickablePlayerRects;
        int mouseX = 0;
        int mouseY = 0;

        // Fade Effect
        bool isFading = false;
        float fadeAlpha = 0.0f;
        float fadeSpeed = 2.0f; // Speed of fade effect

        // Game Timer
        Uint32 gameStartTime = 0;
        bool gameTimerActive = false;
        bool showRealTime = true; // Toggle between real time and game time
        int timeUnit = 100;      // f value from the subject (default 100)

        // Pause Menu
        bool isPaused = false;

        // Help System
        bool showingHelp = false;
        int helpPage = 0; // 0=Lore, 1=Rules, 2=GUI Controls
        const int maxHelpPages = 3;

        // Parallax Background
        SDL_Texture *parallaxTexture = nullptr;
        float bgOffsetX = 0.f;
        float bgOffsetY = 0.f;
        const float bgSpeedX = 20.f;
        const float bgSpeedY = 10.f;

        // Game State Enum
        enum class GameState { MENU, GAME };
        GameState state = GameState::MENU;

        // Layout Management
        LayoutManager layoutManager;
        LayoutMetrics currentLayout;

        // Music Tracking
        bool snoopMusicChecked = false;
        bool musicEnabled = true;

        // SNOOP_DOG Easter Egg
        bool snoopModeActive = false;
        SDL_Texture* snoopParallaxTexture = nullptr;
        SDL_Texture* snoopResourceTextures[7] = {nullptr};

        std::unique_ptr<TweenSystem> tweenSystem;
};
