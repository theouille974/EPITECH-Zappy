/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin [WSL: Ubuntu]
** File description:
** App
*/

#include "App.hpp"
#include "Config.hpp"
#include <iostream>
#include "ServerUpdateManager.hpp"
#include "BoardRenderer.hpp"
#include "TextRenderer.hpp"
#include "Theme.hpp"
#include <algorithm>
#include <vector>
#include <SDL2/SDL_image.h>
#include <map>
#include <unordered_map>
#include <memory>
#include <cctype>
#include <regex>

#define THEME_COUNT 6

bool isValidIPv4(const std::string& ip) {
    std::regex ipv4Pattern(R"(^(\d{1,3})\.(\d{1,3})\.(\d{1,3})\.(\d{1,3})$)");
    std::smatch match;
    if (!std::regex_match(ip, match, ipv4Pattern)) {
        return false;
    }
    
    for (int i = 1; i <= 4; ++i) {
        int octet = std::stoi(match[i].str());
        if (octet < 0 || octet > 255) {
            return false;
        }
    }
    return true;
}

bool isValidIPv4Char(char c) {
    return std::isdigit(c) || c == '.';
}

App::App(const std::string& hostname, int port)
    : serverHostname(hostname), serverPort(port), server(hostname, port) {
    inputHostname = hostname;
    inputPort = std::to_string(port);
    initializeApp();
}

App::App()
    : serverHostname("localhost"), serverPort(4242), server("localhost", 4242) {
    initializeApp();
}

void App::initializeApp() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << std::endl;
        std::exit(1);
    }


    if (IMG_Init(IMG_INIT_PNG) == 0) {
        std::cerr << "SDL_image Init Error: " << IMG_GetError() << std::endl;
        std::exit(1);
    }

    const auto& res = Config::SUPPORTED_RESOLUTIONS[resolutionIndex];

    window = SDL_CreateWindow(
        Config::WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        res.width, res.height,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        std::exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        std::exit(1);
    }

    if (Config::ENABLE_VSYNC) {
        SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    effectSystem = std::make_unique<EffectSystem>(renderer);

    // Load custom cursor
    cursorSurface = IMG_Load("src/gui/assets/cursor.png");
    if (cursorSurface) {
        cursorTexture = SDL_CreateTextureFromSurface(renderer, cursorSurface);
        if (!cursorTexture) {
            std::cerr << "Failed to create cursor texture: " << SDL_GetError() << std::endl;
        }
    } else {
        std::cerr << "Failed to load cursor image: " << IMG_GetError() << std::endl;
    }

    SDL_ShowCursor(SDL_DISABLE);

    audio = std::make_unique<AudioManager>();
    audio->loadSFX("incant", "src/gui/assets/extraball.ogg");
    audio->loadSFX("login", "src/gui/assets/tonesuperhi.ogg");
    audio->loadSFX("logout", "src/gui/assets/scorecounter.ogg");
    audio->loadSFX("drop", "src/gui/assets/ball_add.ogg");
    audio->loadSFX("click", "src/gui/assets/click.mp3");

    bool defaultLoaded = audio->loadMusic("default", "src/gui/assets/default.ogg");
    bool snoopLoaded = audio->loadMusic("snoop", "src/gui/assets/snoop.ogg");

    std::cout << "Music loading status:" << std::endl;
    std::cout << "  - default.ogg: " << (defaultLoaded ? "OK" : "FAILED") << std::endl;
    std::cout << "  - snoop.ogg: " << (snoopLoaded ? "OK" : "FAILED") << std::endl;

    audio->setMusicVolume(20);

    board = std::make_unique<BoardRenderer>(renderer);
    text = std::make_unique<TextRenderer>(renderer, "src/gui/assets/ARIAL.TTF", 20);
    if (board && text) {
        board->setTextRenderer(text.get());
    }

    tweenSystem = std::make_unique<TweenSystem>();
    playerAnimator = std::make_unique<PlayerAnimator>(renderer, *tweenSystem);

    sidebarRenderer = std::make_unique<SidebarRenderer>(renderer, text.get());

    SDL_Surface* bgSurf = IMG_Load("src/gui/assets/BackdropBlackLittleSparkBlack.png");
    if (bgSurf) {
        parallaxTexture = SDL_CreateTextureFromSurface(renderer, bgSurf);
        SDL_FreeSurface(bgSurf);
        if (parallaxTexture) {
            SDL_SetTextureBlendMode(parallaxTexture, SDL_BLENDMODE_BLEND);
            SDL_SetTextureAlphaMod(parallaxTexture, 100); // ~40% opacity
        } else {
            std::cerr << "Failed to create parallax texture: " << SDL_GetError() << std::endl;
        }
    } else {
        std::cerr << "Failed to load parallax background: " << IMG_GetError() << std::endl;
    }

    lastFrameTime = SDL_GetTicks();

    SDL_StartTextInput();
}

App::~App() {
    if (cursorTexture) {
        SDL_DestroyTexture(cursorTexture);
    }
    if (cursorSurface) {
        SDL_FreeSurface(cursorSurface);
    }
    if (parallaxTexture) {
        SDL_DestroyTexture(parallaxTexture);
    }
    if (snoopParallaxTexture) {
        SDL_DestroyTexture(snoopParallaxTexture);
    }
    for (int i = 0; i < 7; ++i) {
        if (snoopResourceTextures[i]) {
            SDL_DestroyTexture(snoopResourceTextures[i]);
        }
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_StopTextInput();
    IMG_Quit();
    SDL_Quit();
}

void App::run() {
    bool running = true;
    while (running) {
        Uint32 currentTime = SDL_GetTicks();
        float dt = (currentTime - lastFrameTime) / 1000.0f;
        lastFrameTime = currentTime;

        handleEvents(running);
        update(dt);
        render();

        if (!Config::ENABLE_VSYNC) {
            Uint32 frameTime = SDL_GetTicks() - currentTime;
            Uint32 delay = 1000 / Config::TARGET_FPS;
            if (frameTime < delay) {
                SDL_Delay(delay - frameTime);
            }
        }
    }
}

void App::handleEvents(bool& running) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT)
            running = false;
        else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    if (showingHelp) {
                        showingHelp = false;
                    } else if (state == GameState::GAME) {
                        isPaused = !isPaused;
                    } else {
                        running = false;
                    }
                    break;
                case SDLK_LEFT:
                    if (showingHelp && helpPage > 0) {
                        helpPage--;
                    }
                    break;
                case SDLK_RIGHT:
                    if (showingHelp && helpPage < maxHelpPages - 1) {
                        helpPage++;
                    }
                    break;
                case SDLK_f:
                    fullscreen = !fullscreen;
                    SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
                    break;
                case SDLK_t:
                    if (GUI::ThemeManager::getTheme().sidebarBorder.r == 140) {
                        struct GUI::Theme classicTheme;
                        classicTheme.windowBackground = {20, 20, 30, 255};
                        classicTheme.sidebarBg = {30, 30, 40, 200};
                        GUI::ThemeManager::loadTheme(classicTheme);
                    } else {
                        GUI::ThemeManager::loadTheme(GUI::getGameCubeTheme());
                    }
                    break;
                case SDLK_r:
                    if (!fullscreen) toggleResolution();
                    break;
                case SDLK_BACKSPACE:
                    if (state == GameState::MENU && !showingHelp && !isFading) {
                        if (editingHostname && !inputHostname.empty())
                            inputHostname.pop_back();
                        else if (editingPort && !inputPort.empty())
                            inputPort.pop_back();
                    }
                    break;
                default: break;
            }
        } else if (e.type == SDL_MOUSEMOTION) {
            mouseX = e.motion.x;
            mouseY = e.motion.y;
        } else if (e.type == SDL_MOUSEWHEEL) {
            if (state == GameState::GAME && layoutManager.isPointInSidebarArea(mouseX, mouseY, currentLayout)) {
                int scrollPixels = -e.wheel.y * 30;
                if (sidebarRenderer) {
                    sidebarRenderer->scroll(scrollPixels);
                }
            }
        } else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            if (audio) audio->playSFX("click");

            int winW, winH;
            SDL_GetWindowSize(window, &winW, &winH);

            if (state == GameState::GAME) {
                if (layoutManager.isPointInBoardArea(e.button.x, e.button.y, currentLayout)) {
                    SDL_Point boardPos = layoutManager.screenToBoard(e.button.x, e.button.y, currentLayout);
                    if (boardPos.x >= 0 && boardPos.y >= 0) {
                        effectSystem->addTileBurst(boardPos.x, boardPos.y, {255, 255, 100, 255}, 8, 0.4f);
                    }
                }
            }

            if (showingHelp) {
                int navBtnW = 100, navBtnH = 40;
                int prevBtnX = winW/2 - navBtnW - 10;
                int nextBtnX = winW/2 + 10;
                int navBtnY = winH - 80;

                bool clickedPrev = (e.button.x >= prevBtnX && e.button.x <= prevBtnX + navBtnW &&
                                   e.button.y >= navBtnY && e.button.y <= navBtnY + navBtnH);
                bool clickedNext = (e.button.x >= nextBtnX && e.button.x <= nextBtnX + navBtnW &&
                                   e.button.y >= navBtnY && e.button.y <= navBtnY + navBtnH);

                if (clickedPrev && helpPage > 0) {
                    helpPage--;
                } else if (clickedNext && helpPage < maxHelpPages - 1) {
                    helpPage++;
                } else if (!clickedPrev && !clickedNext) {
                    showingHelp = false;
                }
                continue;
            }

            if (state == GameState::MENU) {
                float uiScale = std::min(1.0f, winH / 900.0f);

                int fieldW = static_cast<int>(300 * uiScale);
                int fieldH = static_cast<int>(40 * uiScale);
                int btnW = static_cast<int>(220 * uiScale);
                int btnH = static_cast<int>(60 * uiScale);
                int btnX = (winW - btnW) / 2;
                int btnY = (winH - btnH) / 2;

                int fieldX = (winW - fieldW) / 2;
                int hostFieldY = btnY - static_cast<int>(120 * uiScale);
                int portFieldY = hostFieldY + fieldH + 10;

                if (e.button.x >= fieldX && e.button.x <= fieldX + fieldW &&
                    e.button.y >= hostFieldY && e.button.y <= hostFieldY + fieldH) {
                    editingHostname = true;
                    editingPort = false;
                    continue;
                }

                if (e.button.x >= fieldX && e.button.x <= fieldX + fieldW &&
                    e.button.y >= portFieldY && e.button.y <= portFieldY + fieldH) {
                    editingHostname = false;
                    editingPort = true;
                    continue;
                }

                if (e.button.x >= btnX && e.button.x <= btnX + btnW &&
                    e.button.y >= btnY && e.button.y <= btnY + btnH) {
                    if (!isFading) {
                        isFading = true;
                        fadeAlpha = 0.0f;
                    }
                }

                int exitBtnY = btnY + btnH + 20;
                if (e.button.x >= btnX && e.button.x <= btnX + btnW &&
                    e.button.y >= exitBtnY && e.button.y <= exitBtnY + btnH) {
                    running = false;
                }

                int helpBtnY = exitBtnY + btnH + 20;
                if (e.button.x >= btnX && e.button.x <= btnX + btnW &&
                    e.button.y >= helpBtnY && e.button.y <= helpBtnY + btnH) {
                    showingHelp = true;
                    helpPage = 0;
                }

                int squareSize = std::max(16, static_cast<int>(40 * uiScale));
                int gap = static_cast<int>(20 * uiScale);
                int totalW = squareSize * THEME_COUNT + gap * (THEME_COUNT - 1);
                int themeRowY = helpBtnY + btnH + 40;
                int startX = (winW - totalW)/2;
                if (e.button.y >= themeRowY && e.button.y <= themeRowY + squareSize) {
                    for (int i = 0; i < THEME_COUNT; ++i) {
                        int sqX = startX + i*(squareSize + gap);
                        if (e.button.x >= sqX && e.button.x <= sqX + squareSize) {
                            switch (i) {
                                case 0: GUI::ThemeManager::loadTheme(GUI::getGameCubeTheme()); break;
                                case 1: GUI::ThemeManager::loadTheme(GUI::getGameboyPinkTheme()); break;
                                case 2: GUI::ThemeManager::loadTheme(GUI::getMatrixTheme()); break;
                                case 3: GUI::ThemeManager::loadTheme(GUI::getN64Theme()); break;
                                case 4: GUI::ThemeManager::loadTheme(GUI::getRedTheme()); break;
                                case 5: GUI::ThemeManager::loadTheme(GUI::getWhiteTheme()); break;
                            }
                            break;
                        }
                    }
                }

                continue;
            }

            if (state == GameState::GAME && isPaused) {
                float uiScale = std::min(1.0f, winH / 900.0f);

                int btnW = static_cast<int>(220 * uiScale);
                int btnH = static_cast<int>(60 * uiScale);
                int btnX = (winW - btnW) / 2;
                int btnY = (winH - btnH) / 2;

                if (e.button.x >= btnX && e.button.x <= btnX + btnW &&
                    e.button.y >= btnY && e.button.y <= btnY + btnH) {
                    isPaused = false;
                    continue;
                }

                int musicBtnY = btnY + btnH + 20;
                if (e.button.x >= btnX && e.button.x <= btnX + btnW &&
                    e.button.y >= musicBtnY && e.button.y <= musicBtnY + btnH) {
                    musicEnabled = !musicEnabled;
                    if (musicEnabled) {
                        if (snoopMusicChecked) {
                            bool hasSnoopDog = false;
                            for (const auto& team : server.getTeamNames()) {
                                if (team == "SNOOP_DOG") {
                                    hasSnoopDog = true;
                                    break;
                                }
                            }
                            audio->playMusic(hasSnoopDog ? "snoop" : "default", -1);
                        } else {
                            audio->playMusic("default", -1);
                        }
                    } else {
                        audio->stopMusic();
                    }
                    continue;
                }

                int exitBtnY = musicBtnY + btnH + 20;
                if (e.button.x >= btnX && e.button.x <= btnX + btnW &&
                    e.button.y >= exitBtnY && e.button.y <= exitBtnY + btnH) {
                    server.disconnect();
                    audio->stopMusic();
                    
                    server = ServerUpdateManager(serverHostname, static_cast<uint16_t>(serverPort));
                    
                    state = GameState::MENU;
                    isPaused = false;
                    gameTimerActive = false;
                    snoopMusicChecked = false;
                    snoopModeActive = false;

                    if (board) {
                        board->setSnoopMode(false, nullptr);
                    }
                    if (sidebarRenderer) {
                        sidebarRenderer->setSnoopMode(false, nullptr);
                    }

                    selectedX = -1;
                    selectedY = -1;
                    selectedPlayerId = -1;
                    clickablePlayerRects.clear();

                    if (sidebarRenderer) {
                        sidebarRenderer->scroll(-1000);
                    }

                    continue;
                }

                int helpBtnY = exitBtnY + btnH + 20;
                if (e.button.x >= btnX && e.button.x <= btnX + btnW &&
                    e.button.y >= helpBtnY && e.button.y <= helpBtnY + btnH) {
                    showingHelp = true;
                    helpPage = 0;
                    continue;
                }

                int squareSize2 = std::max(16, static_cast<int>(40 * uiScale));
                int gap2 = static_cast<int>(20 * uiScale);
                int totalW2 = squareSize2 * THEME_COUNT + gap2 * (THEME_COUNT - 1);
                int themeRowY2 = helpBtnY + btnH + 40;
                int startX2 = (winW - totalW2)/2;
                if (e.button.y >= themeRowY2 && e.button.y <= themeRowY2 + squareSize2) {
                    for (int i=0;i<THEME_COUNT;++i){
                        int sqX = startX2 + i*(squareSize2+gap2);
                        if (e.button.x >= sqX && e.button.x <= sqX + squareSize2) {
                            switch(i){
                                case 0: GUI::ThemeManager::loadTheme(GUI::getGameCubeTheme()); break;
                                case 1: GUI::ThemeManager::loadTheme(GUI::getGameboyPinkTheme()); break;
                                case 2: GUI::ThemeManager::loadTheme(GUI::getMatrixTheme()); break;
                                case 3: GUI::ThemeManager::loadTheme(GUI::getN64Theme()); break;
                                case 4: GUI::ThemeManager::loadTheme(GUI::getRedTheme()); break;
                                case 5: GUI::ThemeManager::loadTheme(GUI::getWhiteTheme()); break;
                            }
                            break;
                        }
                    }
                }

                continue;
            }

            if (state == GameState::GAME && !isPaused) {
                if (layoutManager.isPointInBoardArea(e.button.x, e.button.y, currentLayout)) {
                    SDL_Point boardPos = layoutManager.screenToBoard(e.button.x, e.button.y, currentLayout);
                    if (boardPos.x >= 0 && boardPos.y >= 0) {
                        selectedX = boardPos.x;
                        selectedY = boardPos.y;
                        const auto& players = server.getPlayers();
                        int foundId = -1;
                        for (const auto& [pid, ply] : players) {
                            if (ply.x == selectedX && ply.y == selectedY && ply.alive) { foundId = pid; break; }
                        }
                        if (foundId != -1) selectedPlayerId = foundId;
                    }
                } else if (layoutManager.isPointInSidebarArea(e.button.x, e.button.y, currentLayout)) {
                    if (sidebarRenderer && sidebarRenderer->isTimerClicked(e.button.x, e.button.y)) {
                        sidebarRenderer->toggleTimerMode();
                        continue;
                    }

                    selectedPlayerId = sidebarRenderer->getPlayerIdAt(e.button.x, e.button.y, currentLayout, clickablePlayerRects);
                }
            }
        } else if (e.type == SDL_TEXTINPUT) {
            if (state == GameState::MENU && !showingHelp && !isFading) {
                if (editingHostname) {
                    if (isValidIPv4Char(e.text.text[0]) && inputHostname.size() < 15) {
                        inputHostname += e.text.text;
                    }
                } else if (editingPort) {
                    if (std::isdigit(e.text.text[0]) && inputPort.size() < 5) {
                        inputPort += e.text.text;
                    }
                }
            }
        }
    }
}

void App::update(float dt) {
    if (isFading && state == GameState::MENU) {
        fadeAlpha += fadeSpeed * dt;
        if (fadeAlpha >= 1.0f) {
            fadeAlpha = 1.0f;

            bool gameStarted = startGame();
            if (gameStarted) {
                isFading = false;
            } else {
                isFading = false;
                fadeAlpha = 0.0f;
            }
        }
    }

    if (state == GameState::GAME && !isPaused) {
        server.poll();

        if (board && server.getBoardWidth() > 0 && server.getBoardHeight() > 0) {
            board->setBoardSize(server.getBoardWidth(), server.getBoardHeight());
        }

        if (!snoopMusicChecked && server.getTeamNames().size() > 0) {
            std::cout << "Checking for SNOOP_DOG team. Found " << server.getTeamNames().size() << " teams:" << std::endl;
            for (const auto& team : server.getTeamNames()) {
                std::cout << "  - " << team << std::endl;
                if (team == "SNOOP_DOG") {
                    std::cout << "Found SNOOP_DOG team! Activating Snoop Dogg easter egg..." << std::endl;
                    
                    if (loadSnoopAssets()) {
                        snoopModeActive = true;
                        if (board) {
                            board->setSnoopMode(true, snoopResourceTextures);
                        }
                        if (sidebarRenderer) {
                            sidebarRenderer->setSnoopMode(true, snoopResourceTextures);
                        }
                        std::cout << "SNOOP_DOG mode activated! Background and resources replaced." << std::endl;
                    } else {
                        std::cerr << "Failed to load SNOOP_DOG assets, using default assets." << std::endl;
                    }
                    
                    if (musicEnabled) {
                        audio->stopMusic();
                        audio->playMusic("snoop", -1);
                    }
                    break;
                }
            }
            snoopMusicChecked = true;
        }

        for (const auto& ev : server.consumeEvents()) {
            switch (ev.type) {
                case ServerUpdateManager::EventType::PLAYER_LEVEL_UP: {
                    SDL_Color spark{255, 220, 40, 255};
                    effectSystem->addTileBurst(ev.x, ev.y, spark, 30, 0.8f, false);
                    audio->playSFX("incant");
                    break; }
                case ServerUpdateManager::EventType::PLAYER_LOGIN:
                    audio->playSFX("login");
                    break;
                case ServerUpdateManager::EventType::PLAYER_LOGOUT:
                    audio->playSFX("logout");
                    if (selectedPlayerId == ev.id) {
                        selectedPlayerId = -1;
                    }
                    break;
                case ServerUpdateManager::EventType::RESOURCE_DROP:
                    audio->playSFX("drop");
                    break;
                case ServerUpdateManager::EventType::INCANTATION_START:
                    break;
                case ServerUpdateManager::EventType::BROADCAST: {
                    SDL_Color green{50, 255, 100, 255};
                    effectSystem->addTileRing(ev.x, ev.y, green, 1.5f, 0.8f, true);
                    audio->playSFX("broadcast");
                    break; }
            }
        }
    }

    if (playerAnimator) {
        playerAnimator->syncWithServer(server);
        playerAnimator->update(dt);
    }

    effectSystem->update(dt);

    bgOffsetX += bgSpeedX * dt;
    bgOffsetY += bgSpeedY * dt;
    const float texSize = 512.f;
    if (bgOffsetX >= texSize) bgOffsetX -= texSize;
    if (bgOffsetY >= texSize) bgOffsetY -= texSize;
}

void App::render() {
    const auto& theme = GUI::ThemeManager::getTheme();

    int winW, winH;
    SDL_GetWindowSize(window, &winW, &winH);
    float uiScale = std::min(1.0f, winH / 900.0f);

    if (state == GameState::MENU) {
        SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
        SDL_RenderClear(renderer);

        drawParallaxBackground();

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        if (text) {
            text->setScaleFactor(5.0f);
            text->drawText("Zappy", winW/2 - 50, winH/4 - 50, {255, 255, 255, 255});

            text->setScaleFactor(1.5f);
            text->drawText("The elevation ritual", winW/2 - 125, winH/4 + 40, {200, 200, 200, 255});

            text->setScaleFactor(1.0f);
        }

        int btnW = static_cast<int>(220 * uiScale);
        int btnH = static_cast<int>(60 * uiScale);
        int btnX = (winW - btnW) / 2;
        int btnY = (winH - btnH) / 2;

        bool hoveringStart = (mouseX >= btnX && mouseX <= btnX + btnW &&
                             mouseY >= btnY && mouseY <= btnY + btnH);

        if (hoveringStart) {
            drawRoundedRect(btnX, btnY, btnW, btnH, 10, 60, 120, 60, 180);
        } else {
            drawRoundedRect(btnX, btnY, btnW, btnH, 10, 40, 90, 40, 150);
        }

        if (text) {
            text->drawText("<-- START -->", btnX + btnW/2 - 50, btnY + btnH/2 - 10, theme.text);
        }

        int exitBtnY = btnY + btnH + 20;
        bool hoveringExit = (mouseX >= btnX && btnX + btnW &&
                            mouseY >= exitBtnY && mouseY <= exitBtnY + btnH);

        if (hoveringExit) {
            drawRoundedRect(btnX, exitBtnY, btnW, btnH, 10, 120, 60, 60, 180);
        } else {
            drawRoundedRect(btnX, exitBtnY, btnW, btnH, 10, 90, 40, 40, 150);
        }

        if (text) {
            text->drawText("<-- EXIT -->", btnX + btnW/2 - 50, exitBtnY + btnH/2 - 10, theme.text);
        }

        int helpBtnY = exitBtnY + btnH + 20;
        bool hoveringHelp = (mouseX >= btnX && btnX + btnW &&
                            mouseY >= helpBtnY && mouseY <= helpBtnY + btnH);

        drawHelpButton(btnX, helpBtnY, btnW, btnH, hoveringHelp);

        if (text) {
            text->drawText("<-- HELP -->", btnX + btnW/2 - 50, helpBtnY + btnH/2 - 10, theme.text);
        }

        int squareSize = std::max(16, static_cast<int>(40 * uiScale));
        int gap = static_cast<int>(20 * uiScale);
        int totalW = squareSize * THEME_COUNT + gap * (THEME_COUNT - 1);
        int themeRowY = helpBtnY + btnH + 40;
        int startX = (winW - totalW)/2;
        SDL_Color previews[THEME_COUNT] = {
            GUI::getGameCubeTheme().accent,
            GUI::getGameboyPinkTheme().accent,
            GUI::getMatrixTheme().accent,
            GUI::getN64Theme().accent,
            GUI::getRedTheme().accent,
            GUI::getWhiteTheme().accent
        };
        for (int i = 0; i < THEME_COUNT; ++i) {
            int sqX = startX + i * (squareSize + gap);
            SDL_SetRenderDrawColor(renderer, previews[i].r, previews[i].g, previews[i].b, 255);
            SDL_Rect r{sqX, themeRowY, squareSize, squareSize};
            SDL_RenderFillRect(renderer, &r);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &r);
        }

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

        int fieldW = static_cast<int>(300 * uiScale);
        int fieldH = static_cast<int>(40 * uiScale);
        int fieldX = (winW - fieldW) / 2;
        int hostFieldY = btnY - static_cast<int>(120 * uiScale);
        int portFieldY = hostFieldY + fieldH + 10;

        auto drawInputField=[&](int x,int y,const std::string &content,bool focused,const std::string &placeholder){
            drawRoundedRect(x, y, fieldW, fieldH, 6, 30,30,40,200);
            SDL_Color borderCol = focused ? GUI::ThemeManager::getTheme().accent : GUI::ThemeManager::getTheme().panelBorder;
            SDL_SetRenderDrawColor(renderer, borderCol.r, borderCol.g, borderCol.b, borderCol.a);
            SDL_Rect r{x,y,fieldW,fieldH};
            SDL_RenderDrawRect(renderer, &r);
            if (text) {
                std::string shown = content.empty() ? placeholder : content;
                SDL_Color col = content.empty() ? GUI::ThemeManager::getTheme().textDisabled : GUI::ThemeManager::getTheme().text;
                text->drawText(shown, x + 8, y + fieldH/2 - 10, col);
            }
        };

        drawInputField(fieldX, hostFieldY, inputHostname, editingHostname, "IPv4 address (e.g. 192.168.1.1)");
        drawInputField(fieldX, portFieldY, inputPort, editingPort, "Port (1-65535)");

        if (isFading) {
            Uint8 alpha = static_cast<Uint8>(fadeAlpha * 255);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
            SDL_Rect fullScreen{0, 0, winW, winH};
            SDL_RenderFillRect(renderer, &fullScreen);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }

        if (showingHelp) {
            renderHelpScreen();
        }

        if (cursorTexture) {
            int cursorSize = static_cast<int>(24 * uiScale);
            int hotspotX = 12;
            int hotspotY = 12;
            SDL_Rect cursorRect{mouseX - hotspotX, mouseY - hotspotY, cursorSize, cursorSize};
            SDL_RenderCopy(renderer, cursorTexture, nullptr, &cursorRect);
        }

        SDL_RenderPresent(renderer);
        return;
    }

    SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
    SDL_RenderClear(renderer);

    drawParallaxBackground();

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    currentLayout = layoutManager.calculateLayout(winW, winH, server.getBoardWidth(), server.getBoardHeight());

    if (text) {
        text->setScaleFactor(static_cast<float>(currentLayout.textLineHeight) / 18.0f);
        text->setViewport(0, 0, winW, winH);
    }

    if (text) {
        text->beginBatch();
    }

    if (board) {
        board->render(currentLayout,
            [this](int x, int y){ return server.getTileResources(x, y); },
            [this](int x, int y){ return server.getEggsAt(x, y); },
            selectedX, selectedY);
    }

    effectSystem->render(currentLayout.boardAreaWidth, currentLayout.boardAreaHeight, 
                        server.getBoardWidth(), server.getBoardHeight(), false);

    if (playerAnimator) {
        playerAnimator->render(currentLayout.boardAreaWidth, currentLayout.boardAreaHeight, 
                              server.getBoardWidth(), server.getBoardHeight(), selectedPlayerId, text.get());
    }

    effectSystem->render(currentLayout.boardAreaWidth, currentLayout.boardAreaHeight, 
                        server.getBoardWidth(), server.getBoardHeight(), true);

    if (sidebarRenderer) {
        sidebarRenderer->render(currentLayout, server, selectedX, selectedY, selectedPlayerId, clickablePlayerRects);
    }

    if (state == GameState::GAME && isPaused) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
        SDL_Rect fullScreen{0, 0, winW, winH};
        SDL_RenderFillRect(renderer, &fullScreen);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

        int btnW = static_cast<int>(220 * uiScale);
        int btnH = static_cast<int>(60 * uiScale);
        int btnX = (winW - btnW) / 2;
        int btnY = (winH - btnH) / 2;

        bool hoveringContinue = (mouseX >= btnX && mouseX <= btnX + btnW &&
                                mouseY >= btnY && mouseY <= btnY + btnH);

        int musicBtnY = btnY + btnH + 20;
        bool hoveringMusic = (mouseX >= btnX && mouseX <= btnX + btnW &&
                             mouseY >= musicBtnY && mouseY <= musicBtnY + btnH);

        int exitBtnY = musicBtnY + btnH + 20;
        bool hoveringExit = (mouseX >= btnX && mouseX <= btnX + btnW &&
                           mouseY >= exitBtnY && mouseY <= exitBtnY + btnH);

        if (hoveringContinue) {
            drawRoundedRect(btnX, btnY, btnW, btnH, 10, 80, 140, 80, 200);
        } else {
            drawRoundedRect(btnX, btnY, btnW, btnH, 10, 60, 120, 60, 180);
        }

        if (text) {
            int textX = btnX + btnW/2 - 65;
            int textY = btnY + btnH/2 - 15;
            text->drawText("CONTINUE", textX, textY, {255,255,255,255});
        }

        if (hoveringMusic) {
            drawRoundedRect(btnX, musicBtnY, btnW, btnH, 10, 80, 80, 140, 200);
        } else {
            drawRoundedRect(btnX, musicBtnY, btnW, btnH, 10, 60, 60, 120, 180);
        }

        if (text) {
            std::string musicText = musicEnabled ? "MUSIC: ON" : "MUSIC: OFF";
            int textX = btnX + btnW/2 - 70;
            int textY = musicBtnY + btnH/2 - 15;
            text->drawText(musicText, textX, textY, {255,255,255,255});
        }

        if (hoveringExit) {
            drawRoundedRect(btnX, exitBtnY, btnW, btnH, 10, 140, 80, 80, 200);
        } else {
            drawRoundedRect(btnX, exitBtnY, btnW, btnH, 10, 120, 60, 60, 180);
        }

        if (text) {
            int textX = btnX + btnW/2 - 85;
            int textY = exitBtnY + btnH/2 - 15;
            text->drawText("EXIT TO MENU", textX, textY, {255,255,255,255});
        }

        int helpBtnY = exitBtnY + btnH + 20;
        bool hoveringHelp = (mouseX >= btnX && mouseX <= btnX + btnW &&
                            mouseY >= helpBtnY && mouseY <= helpBtnY + btnH);

        drawHelpButton(btnX, helpBtnY, btnW, btnH, hoveringHelp);

        if (text) {
            int textX = btnX + btnW/2 - 50;
            int textY = helpBtnY + btnH/2 - 15;
            text->drawText("<-- HELP -->", textX, textY, {255,255,255,255});
        }

        int squareSizeP = std::max(16, static_cast<int>(40 * uiScale));
        int gapP = static_cast<int>(20 * uiScale);
        int totalWP = squareSizeP * THEME_COUNT + gapP * (THEME_COUNT - 1);
        int themeRowYP = helpBtnY + btnH + 40;
        int startXP = (winW - totalWP) / 2;
        SDL_Color previewsP[THEME_COUNT] = {
            GUI::getGameCubeTheme().accent,
            GUI::getGameboyPinkTheme().accent,
            GUI::getMatrixTheme().accent,
            GUI::getN64Theme().accent,
            GUI::getRedTheme().accent,
            GUI::getWhiteTheme().accent
        };
        for (int i = 0; i < THEME_COUNT; ++i) {
            int sqX = startXP + i * (squareSizeP + gapP);
            SDL_SetRenderDrawColor(renderer, previewsP[i].r, previewsP[i].g, previewsP[i].b, 255);
            SDL_Rect r{sqX, themeRowYP, squareSizeP, squareSizeP};
            SDL_RenderFillRect(renderer, &r);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &r);
        }
    }

    if (text) {
        text->endBatch();
    }

    if (text) {
        text->clearViewport();
    }

    if (cursorTexture) {
        int cursorSize = static_cast<int>(24 * uiScale);
        int hotspotX = 12;
        int hotspotY = 12;
        SDL_Rect cursorRect{mouseX - hotspotX, mouseY - hotspotY, cursorSize, cursorSize};
        SDL_RenderCopy(renderer, cursorTexture, nullptr, &cursorRect);
    }

    if (showingHelp) {
        renderHelpScreen();

        if (cursorTexture) {
            int cursorSize = static_cast<int>(24 * uiScale);
            int hotspotX = 12;
            int hotspotY = 12;
            SDL_Rect cursorRect{mouseX - hotspotX, mouseY - hotspotY, cursorSize, cursorSize};
            SDL_RenderCopy(renderer, cursorTexture, nullptr, &cursorRect);
        }
    }

    SDL_RenderPresent(renderer);
}

void App::applyResolution(int index) {
    resolutionIndex = index;

    const auto& res = Config::SUPPORTED_RESOLUTIONS[resolutionIndex];
    SDL_SetWindowSize(window, res.width, res.height);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

void App::toggleResolution() {
    resolutionIndex = (resolutionIndex + 1) % Config::SUPPORTED_RESOLUTIONS.size();
    applyResolution(resolutionIndex);
}

bool App::startGame() {
    if (state == GameState::GAME)
        return true;

    int portValue = serverPort;
    try {
        portValue = std::stoi(inputPort);
    } catch (const std::exception&) {}

    if (inputHostname.empty()) {
        serverHostname = serverHostname;
    } else if (isValidIPv4(inputHostname)) {
        serverHostname = inputHostname;
    } else {
        serverHostname = "127.0.0.1";
        inputHostname = "127.0.0.1";
    }
    serverPort = portValue;

    server = ServerUpdateManager(serverHostname, static_cast<uint16_t>(serverPort));

    if (!server.connect()) {
        std::cerr << "Failed to connect to server " << serverHostname << ":" << serverPort << std::endl;
        return false;
    }

    state = GameState::GAME;
    gameStartTime = SDL_GetTicks();
    gameTimerActive = true;
    snoopMusicChecked = false;
    snoopModeActive = false;

    if (board) {
        board->setSnoopMode(false, nullptr);
    }
    if (sidebarRenderer) {
        sidebarRenderer->setSnoopMode(false, nullptr);
    }

    selectedX = -1;
    selectedY = -1;
    selectedPlayerId = -1;
    clickablePlayerRects.clear();

    if (sidebarRenderer) {
        sidebarRenderer->scroll(-1000);
    }

    if (musicEnabled) {
        audio->playMusic("default", -1);
    }

    if (sidebarRenderer) {
        sidebarRenderer->setGameStartTime(gameStartTime);
    }

    return true;
}

void App::drawFilledCircle(int centerX, int centerY, int radius) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) {
                SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
            }
        }
    }
}

void App::drawRoundedRect(int x, int y, int w, int h, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);

    SDL_Rect topRect{x + radius, y, w - 2 * radius, radius};
    SDL_Rect middleRect{x, y + radius, w, h - 2 * radius};
    SDL_Rect bottomRect{x + radius, y + h - radius, w - 2 * radius, radius};

    SDL_RenderFillRect(renderer, &topRect);
    SDL_RenderFillRect(renderer, &middleRect);
    SDL_RenderFillRect(renderer, &bottomRect);

    drawFilledCircle(x + radius, y + radius, radius);
    drawFilledCircle(x + w - radius, y + radius, radius);
    drawFilledCircle(x + radius, y + h - radius, radius);
    drawFilledCircle(x + w - radius, y + h - radius, radius);
}

void App::drawLifeBar(int x, int y, int width, int height, float lifePercentage) {
    lifePercentage = std::max(0.0f, std::min(1.0f, lifePercentage));

    SDL_SetRenderDrawColor(renderer, 80, 20, 20, 255);
    SDL_Rect bgRect{x, y, width, height};
    SDL_RenderFillRect(renderer, &bgRect);

    int lifeWidth = static_cast<int>(width * lifePercentage);
    if (lifeWidth > 0) {
        SDL_Color lifeColor;
        if (lifePercentage > 0.6f) {
            lifeColor = {0, 200, 0, 255};
        } else if (lifePercentage > 0.3f) {
            lifeColor = {200, 200, 0, 255};
        } else {
            lifeColor = {200, 0, 0, 255};
        }

        SDL_SetRenderDrawColor(renderer, lifeColor.r, lifeColor.g, lifeColor.b, lifeColor.a);
        SDL_Rect lifeRect{x, y, lifeWidth, height};
        SDL_RenderFillRect(renderer, &lifeRect);
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &bgRect);
}

void App::drawPlayer(int x, int y, int radius, const SDL_Color& color, float lifePercentage, bool isMultiple, int playerId, int orientation) {
    if (isMultiple) {
        radius = static_cast<int>(radius * 0.7f);
    }

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_Rect rect{x - radius, y - radius, radius * 2, radius * 2};
    SDL_RenderFillRect(renderer, &rect);

    int lifeBarWidth = radius * 2;
    int lifeBarHeight = 8;
    int lifeBarX = x - radius;
    int lifeBarY = y - radius - lifeBarHeight - 2;

    drawLifeBar(lifeBarX, lifeBarY, lifeBarWidth, lifeBarHeight, lifePercentage);

    if (text) {
        std::string idText = "#" + std::to_string(playerId);
        int textX = x - radius + 2;
        int textY = y - radius + 2;
        text->drawText(idText, textX, textY, {255, 255, 255, 255});
    }

    drawDirectionIndicator(x, y, radius, orientation);
}

void App::drawDirectionIndicator(int x, int y, int radius, int orientation) {
    int indicatorRadius = radius / 3;
    if (indicatorRadius < 2) indicatorRadius = 2;

    int offsetX = 0, offsetY = 0;

    switch (orientation) {
        case 1: // North
            offsetY = -radius - indicatorRadius;
            break;
        case 2: // East
            offsetX = radius + indicatorRadius;
            break;
        case 3: // South
            offsetY = radius + indicatorRadius;
            break;
        case 4: // West
            offsetX = -radius - indicatorRadius;
            break;
        default:
            return;
    }

    int indicatorX = x + offsetX;
    int indicatorY = y + offsetY;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect indicatorRect{indicatorX - indicatorRadius, indicatorY - indicatorRadius, 
                          indicatorRadius * 2, indicatorRadius * 2};
    SDL_RenderFillRect(renderer, &indicatorRect);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &indicatorRect);
}

void App::drawParallaxBackground() {
    SDL_Texture* currentTexture = snoopModeActive && snoopParallaxTexture ? snoopParallaxTexture : parallaxTexture;
    if (!currentTexture) return;

    int winW, winH;
    SDL_GetWindowSize(window, &winW, &winH);

    int texW = 512;
    int texH = 512;

    int startX = static_cast<int>(-bgOffsetX);
    int startY = static_cast<int>(-bgOffsetY);

    SDL_Rect dst{0,0,texW,texH};
    for (int y = startY; y < winH; y += texH) {
        dst.y = y;
        for (int x = startX; x < winW; x += texW) {
            dst.x = x;
            SDL_RenderCopy(renderer, currentTexture, nullptr, &dst);
        }
    }
}

void App::drawHelpButton(int x, int y, int w, int h, bool hovering) {
    if (hovering) {
        drawRoundedRect(x, y, w, h, 10, 100, 80, 140, 180);
    } else {
        drawRoundedRect(x, y, w, h, 10, 80, 60, 120, 150);
    }
}

void App::renderHelpScreen() {
    if (!text) return;

    int winW, winH;
    SDL_GetWindowSize(window, &winW, &winH);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_Rect fullScreen{0, 0, winW, winH};
    SDL_RenderFillRect(renderer, &fullScreen);

    int panelW = winW - 100;
    int panelH = winH - 100;
    int panelX = 50;
    int panelY = 50;

    drawRoundedRect(panelX, panelY, panelW, panelH, 15, 30, 30, 50, 240);
    SDL_SetRenderDrawColor(renderer, 100, 100, 120, 255);
    SDL_Rect panelRect{panelX, panelY, panelW, panelH};
    SDL_RenderDrawRect(renderer, &panelRect);

    int textX = panelX + 30;
    int textY = panelY + 30;
    int lineHeight = 25;

    std::string pageTitle;
    std::vector<std::string> content;

    switch (helpPage) {
        case 0:
            pageTitle = "ZAPPY - The Elevation Ritual";
            content = {
                "In the mystical world of Zappy, ancient beings known as",
                "Trantorians seek spiritual elevation through sacred rituals.",
                "",
                "These ethereal creatures inhabit a vast grid-like realm",
                "where precious stones and life-giving food materialize",
                "from the cosmic ether.",
                "",
                "Legend speaks of the 'Elevation Ritual' - a ceremony where",
                "Trantorians of sufficient level and resources can transcend",
                "their current form and ascend to higher planes of existence.",
                "",
                "Teams of Trantorians compete to achieve this transcendence,",
                "guided by mysterious server entities that observe their",
                "every move from dimensions beyond mortal comprehension.",
                "",
                "The first team to achieve 6 elevated beings shall unlock",
                "the secrets of the universe and claim eternal victory!"
            };
            break;

        case 1:
            pageTitle = "Game Rules & Mechanics";
            content = {
                "OBJECTIVE: First team to get 6 players to level 8 wins!",
                "",
                "ELEVATION REQUIREMENTS by Level:",
                "• Level 1→2: 1 player, 1 linemate",
                "• Level 2→3: 2 players, 1 linemate, 1 deraumere, 1 sibur",
                "• Level 3→4: 2 players, 2 linemate, 1 deraumere, 1 sibur",
                "• Level 4→5: 4 players, 1 linemate, 1 deraumere, 2 sibur, 1 phiras",
                "• Level 5→6: 4 players, 1 linemate, 2 deraumere, 1 sibur, 2 phiras",
                "• Level 6→7: 6 players, 1 linemate, 2 deraumere, 3 sibur, 1 phiras",
                "• Level 7→8: 6 players, 2 linemate, 2 deraumere, 2 sibur, 2 phiras, 2 thystame",
                "",
                "RESOURCES:",
                "• Food: Required for survival (life meter)",
                "• Stones: Linemate, Deraumere, Sibur, Mendiane, Phiras, Thystame",
                "",
                "MECHANICS:",
                "• Players spawn as eggs and hatch after time units",
                "• Life decreases over time, eat food to survive",
                "• Resources spawn randomly across the map",
                "• Incantations require exact player count and resources"
            };
            break;

        case 2:
            pageTitle = "GUI Controls & Features";
            content = {
                "MOUSE CONTROLS:",
                "• Click tiles to select and view tile information",
                "• Click players in sidebar to track specific player",
                "• Scroll wheel in sidebar to navigate content",
                "• Click timer to toggle between real/game time",
                "",
                "KEYBOARD SHORTCUTS:",
                "• ESC: Pause game / Close help",
                "• F: Toggle fullscreen mode",
                "• R: Cycle through resolutions (windowed mode)",
                "• T: Quick theme toggle",
                "• Arrow Keys: Navigate help pages",
                "",
                "VISUAL ELEMENTS:",
                "• Player squares show team colors and IDs",
                "• Life bars above players indicate health",
                "• White squares show player facing direction",
                "• Colored icons represent different resources",
                "• Particle effects mark incantations and events",
                "",
                "SIDEBAR INFO:",
                "• Timer: Shows elapsed time (click to toggle mode)",
                "• Player List: All players organized by team",
                "• Resource Legend: Total resources on map",
                "• Tile Info: Details of selected tile",
                "• Player Monitor: Detailed stats of selected player"
            };
            break;
    }

    text->setScaleFactor(2.0f);
    text->drawText(pageTitle, textX, textY, {255, 255, 255, 255});
    text->setScaleFactor(1.0f);
    textY += lineHeight * 4;

    for (const auto& line : content) {
        if (textY + lineHeight > panelY + panelH - 80) break;
        text->drawText(line, textX, textY, {200, 200, 200, 255});
        textY += lineHeight;
    }

    int navBtnW = 100, navBtnH = 40;
    int prevBtnX = winW/2 - navBtnW - 10;
    int nextBtnX = winW/2 + 10;
    int navBtnY = winH - 80;

    bool hoveringPrev = (mouseX >= prevBtnX && mouseX <= prevBtnX + navBtnW &&
                        mouseY >= navBtnY && mouseY <= navBtnY + navBtnH);
    if (helpPage > 0) {
        drawRoundedRect(prevBtnX, navBtnY, navBtnW, navBtnH, 8,
                       hoveringPrev ? 80 : 60, hoveringPrev ? 80 : 60,
                       hoveringPrev ? 120 : 100, 200);
        text->drawText("< Previous", prevBtnX + 10, navBtnY + 12, {255, 255, 255, 255});
    }

    bool hoveringNext = (mouseX >= nextBtnX && mouseX <= nextBtnX + navBtnW &&
                        mouseY >= navBtnY && mouseY <= navBtnY + navBtnH);
    if (helpPage < maxHelpPages - 1) {
        drawRoundedRect(nextBtnX, navBtnY, navBtnW, navBtnH, 8,
                       hoveringNext ? 80 : 60, hoveringNext ? 80 : 60,
                       hoveringNext ? 120 : 100, 200);
        text->drawText("Next >", nextBtnX + 25, navBtnY + 12, {255, 255, 255, 255});
    }

    std::string pageInfo = std::to_string(helpPage + 1) + " / " + std::to_string(maxHelpPages);
    text->drawText(pageInfo, winW/2 - 20, navBtnY + 50, {150, 150, 150, 255});

    text->drawText("Press ESC or click anywhere to close", winW - winW + 65, panelY + panelH + 10, {150, 150, 150, 255});

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

bool App::loadSnoopAssets() {
    std::cout << "Loading SNOOP_DOG easter egg assets..." << std::endl;
    
    SDL_Surface* bgSurf = IMG_Load("src/gui/assets/snoop_assets/bg.jpg");
    if (bgSurf) {
        snoopParallaxTexture = SDL_CreateTextureFromSurface(renderer, bgSurf);
        SDL_FreeSurface(bgSurf);
        if (snoopParallaxTexture) {
            SDL_SetTextureBlendMode(snoopParallaxTexture, SDL_BLENDMODE_BLEND);
            SDL_SetTextureAlphaMod(snoopParallaxTexture, 100);
            std::cout << "  - Snoop background: OK" << std::endl;
        } else {
            std::cerr << "  - Snoop background: FAILED to create texture" << std::endl;
            return false;
        }
    } else {
        std::cerr << "  - Snoop background: FAILED to load" << std::endl;
        return false;
    }
    
    const char* snoopResourcePaths[7] = {
        "src/gui/assets/snoop_assets/food.png",
        "src/gui/assets/snoop_assets/gem1.png",
        "src/gui/assets/snoop_assets/gem2.png",
        "src/gui/assets/snoop_assets/gem3.png",
        "src/gui/assets/snoop_assets/gem4.png",
        "src/gui/assets/snoop_assets/gem5.png",
        "src/gui/assets/snoop_assets/gem6.png"
    };
    
    bool allResourcesLoaded = true;
    for (int i = 0; i < 7; ++i) {
        SDL_Surface* surf = IMG_Load(snoopResourcePaths[i]);
        if (surf) {
            snoopResourceTextures[i] = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_FreeSurface(surf);
            if (snoopResourceTextures[i]) {
                std::cout << "  - Snoop resource " << i << ": OK" << std::endl;
            } else {
                std::cerr << "  - Snoop resource " << i << ": FAILED to create texture" << std::endl;
                allResourcesLoaded = false;
            }
        } else {
            std::cerr << "  - Snoop resource " << i << ": FAILED to load" << std::endl;
            allResourcesLoaded = false;
        }
    }
    
    if (allResourcesLoaded) {
        std::cout << "SNOOP_DOG assets loaded successfully!" << std::endl;
        return true;
    } else {
        std::cerr << "Some SNOOP_DOG assets failed to load!" << std::endl;
        return false;
    }
}
