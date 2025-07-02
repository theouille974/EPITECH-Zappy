#include "SidebarRenderer.hpp"
#include "TextRenderer.hpp"
#include <SDL2/SDL_image.h>
#include <iostream>
#include <algorithm>

SidebarRenderer::SidebarRenderer(SDL_Renderer* renderer, TextRenderer* textRenderer)
    : _renderer(renderer), _textRenderer(textRenderer) {
    
    _scrollOffset = 0;

    if (!loadResourceSprites()) {
        std::cerr << "Warning: Could not load some sidebar resource sprites, using fallback squares" << std::endl;
    }
}

SidebarRenderer::~SidebarRenderer() {
    for (SDL_Texture*& tex : _resourceTextures) {
        if (tex) {
            SDL_DestroyTexture(tex);
            tex = nullptr;
        }
    }
}

void SidebarRenderer::render(const LayoutMetrics& metrics, 
                            const ServerUpdateManager& server,
                            int selectedX, int selectedY,
                            int selectedPlayerId,
                            const std::vector<std::pair<SDL_Rect, int>>& clickablePlayerRects) {
    
    const auto& theme = GUI::ThemeManager::getTheme();

    SDL_SetRenderDrawColor(_renderer, theme.sidebarBg.r, theme.sidebarBg.g, theme.sidebarBg.b, theme.sidebarBg.a);
    SDL_Rect sidebarRect{metrics.sidebarX, metrics.sidebarY, metrics.sidebarWidth, metrics.sidebarHeight};
    SDL_RenderFillRect(_renderer, &sidebarRect);
    
    SDL_SetRenderDrawColor(_renderer, theme.sidebarBorder.r, theme.sidebarBorder.g, theme.sidebarBorder.b, theme.sidebarBorder.a);
    SDL_RenderDrawRect(_renderer, &sidebarRect);
    
    SDL_Rect playerPanelRect = computePlayerPanelRect(metrics, server, selectedPlayerId);

    if (_textRenderer) {
        _textRenderer->flushBatch();
    }

    SDL_Rect scrollClip = sidebarRect;
    if (playerPanelRect.h > 0) {
        scrollClip.h = playerPanelRect.y - sidebarRect.y;
    }
    SDL_RenderSetClipRect(_renderer, &scrollClip);

    renderGameTimer(metrics, server);
    renderPlayerList(metrics, server, const_cast<std::vector<std::pair<SDL_Rect, int>>&>(clickablePlayerRects));
    renderResourceLegend(metrics, server);
    renderSelectedTileInfo(metrics, server, selectedX, selectedY, selectedPlayerId);

    if (_textRenderer) _textRenderer->flushBatch();

    SDL_RenderSetClipRect(_renderer, nullptr);

    renderSelectedPlayerInfo(metrics, server, selectedPlayerId);
}

void SidebarRenderer::renderGameTimer(const LayoutMetrics& metrics, const ServerUpdateManager& /*server*/) {
    const auto& theme = GUI::ThemeManager::getTheme();
    
    SDL_Rect timerPanel{metrics.sidebarX + metrics.padding, metrics.padding - _scrollOffset, 
                       metrics.sidebarWidth - metrics.padding * 2, metrics.buttonHeight};
    drawPanel(timerPanel, theme.panelBg, theme.panelBorder);
    std::string timeStr;
    if (_gameStartTime > 0) {
        Uint32 currentTime = SDL_GetTicks();
        Uint32 elapsedTime = currentTime - _gameStartTime;
        
        if (_showGameTime) {
            int timeUnitFactor = 100;
            Uint64 gameElapsedTimeMs = static_cast<Uint64>(elapsedTime) * timeUnitFactor;
            
            int seconds = (gameElapsedTimeMs / 1000) % 60;
            int minutes = (gameElapsedTimeMs / 60000) % 60;
            int hours = gameElapsedTimeMs / 3600000;
            
            timeStr = "Game Time: ";
            if (hours > 0) {
                timeStr += std::to_string(hours) + "h ";
            }
            timeStr += std::to_string(minutes) + "m " + std::to_string(seconds) + "s";
        } else {
            int seconds = (elapsedTime / 1000) % 60;
            int minutes = (elapsedTime / 60000) % 60;
            int hours = elapsedTime / 3600000;
            
            timeStr = "Real Time: ";
            if (hours > 0) {
                timeStr += std::to_string(hours) + "h ";
            }
            timeStr += std::to_string(minutes) + "m " + std::to_string(seconds) + "s";
        }
        
        timeStr += " (click)";
    } else {
        timeStr = (_showGameTime ? "Game Time: " : "Real Time: ") + std::string("Not started");
    }
    
    if (_textRenderer) {
        _textRenderer->drawText(timeStr, timerPanel.x + metrics.padding, 
                               timerPanel.y + metrics.padding, theme.textHighlight);
    }
    
    _timerRect = timerPanel;
}

void SidebarRenderer::renderPlayerList(const LayoutMetrics& metrics, const ServerUpdateManager& server,
                                      std::vector<std::pair<SDL_Rect, int>>& clickablePlayerRects) {
    const auto& theme = GUI::ThemeManager::getTheme();
    int y = calculatePlayerListY(metrics) - _scrollOffset;
    
    int availableHeight = metrics.sidebarHeight - y - metrics.padding;
    if (availableHeight < 60) {
        availableHeight = 60;
    }
    
    bool summaryMode = metrics.isCompactMode || availableHeight < 150;
    
    SDL_Rect playerPanel{metrics.sidebarX + metrics.padding, y, 
                        metrics.sidebarWidth - metrics.padding * 2, availableHeight};
    drawPanel(playerPanel, theme.panelBg, theme.panelBorder);
    
    y += metrics.padding;
    clickablePlayerRects.clear();
    
    const auto& players = server.getPlayers();
    
    for (const auto& teamName : server.getTeamNames()) {
        auto it = theme.teamColors.find(teamName);
        SDL_Color teamCol = (it != theme.teamColors.end()) ? it->second : theme.teamColors.at("DEFAULT");
        
        // Team header
        if (summaryMode) {
            // In summary mode, display count of players only
            int count = 0;
            for (const auto& [id, p] : players) {
                if (p.team == teamName) count++;
            }
            std::string label = teamName + ": " + std::to_string(count) + " players";
            if (_textRenderer) {
                _textRenderer->drawText(label, playerPanel.x + metrics.padding, y, teamCol);
            }
            y += metrics.textLineHeight;
            continue;
        }
        
        if (_textRenderer) {
            _textRenderer->drawText(teamName + ":", playerPanel.x + metrics.padding, y, teamCol);
        }
        y += metrics.textLineHeight;
        
        for (const auto& [id, p] : players) {
            if (p.team != teamName) continue;
            
            auto it_player = theme.teamColors.find(p.team);
            SDL_Color col = (it_player != theme.teamColors.end()) ? it_player->second : theme.teamColors.at("DEFAULT");
            std::string label = "#" + std::to_string(id) + " (L" + std::to_string(p.level) + ")";
            if (_textRenderer) {
                _textRenderer->drawText(label, playerPanel.x + metrics.padding * 2, y, col);
            }
            
            float lifePercentage = 1.0f;
            if (p.lifeMeter > 0) lifePercentage = std::min(1.0f, p.lifeMeter / 126.0f);
            int lifeBarWidth = metrics.isCompactMode ? 40 : 60;
            int lifeBarHeight = 6;
            int lifeBarX = playerPanel.x + playerPanel.w - metrics.padding - lifeBarWidth;
            int lifeBarY = y + 2;
            drawProgressBar(lifeBarX, lifeBarY, lifeBarWidth, lifeBarHeight, lifePercentage,
                             theme.accent, theme.panelBorder);
            
            SDL_Rect clickRect{playerPanel.x + metrics.padding * 2, y,
                               playerPanel.w - metrics.padding * 4 - lifeBarWidth, metrics.textLineHeight};
            clickablePlayerRects.emplace_back(clickRect, id);
            
            y += metrics.textLineHeight;
        }
        y += metrics.padding / 2;
    }
}

void SidebarRenderer::renderResourceLegend(const LayoutMetrics& metrics, const ServerUpdateManager& server) {
    const auto& theme = GUI::ThemeManager::getTheme();
    int y = calculateResourceLegendY(metrics, server) - _scrollOffset;

    int tileInfoY = calculateSelectedTileY(metrics, server) - _scrollOffset;
    int availableHeight = tileInfoY - y - metrics.padding;
    if (availableHeight < 50) {
        availableHeight = 50;
    }
    
    int minLegendHeight = metrics.padding + metrics.textLineHeight * 8 + metrics.padding; // header + 7 lines
    if (availableHeight < minLegendHeight)
        availableHeight = minLegendHeight;

    SDL_Rect legendPanel{metrics.sidebarX + metrics.padding, y,
                         metrics.sidebarWidth - metrics.padding * 2, availableHeight};
    drawPanelNoBottom(legendPanel, theme.panelBg, theme.panelBorder);
    
    y += metrics.padding;
    
    if (_textRenderer) {
        _textRenderer->drawText("Legend (total):", legendPanel.x + metrics.padding, y, theme.text);
    }
    y += metrics.textLineHeight;
    
    const char* resNames[7] = {"Food", "Linemate", "Deraumere", "Sibur", "Mendiane", "Phiras", "Thystame"};
    
    auto totalsForLegend = server.getTotalResources();
    
    for (int r = 0; r < 7; ++r) {
        drawResourceIcon(legendPanel.x + metrics.padding, y + 2, 12, r);
        
        std::string legendLine = std::string(resNames[r]) + ": " + std::to_string(totalsForLegend.qty[r]);
        if (_textRenderer) {
            _textRenderer->drawText(legendLine, legendPanel.x + metrics.padding + 20, y, theme.text);
        }
        
        y += metrics.textLineHeight;
    }
}

void SidebarRenderer::renderSelectedTileInfo(const LayoutMetrics& metrics, const ServerUpdateManager& server,
                                            int selectedX, int selectedY, int selectedPlayerId) {
    if (selectedX < 0 || selectedY < 0) return;
    
    const auto& theme = GUI::ThemeManager::getTheme();
    int y = calculateSelectedTileY(metrics, server) - _scrollOffset;
    
    SDL_Rect playerPanelRect = computePlayerPanelRect(metrics, server, selectedPlayerId);
    int bottomLimit = playerPanelRect.y > 0 ? playerPanelRect.y : metrics.sidebarY + metrics.sidebarHeight;
    int availableHeight = bottomLimit - y - metrics.padding;
    
    SDL_Rect tilePanel{metrics.sidebarX + metrics.padding, y, 
                      metrics.sidebarWidth - metrics.padding * 2, availableHeight};
    drawPanelNoBottom(tilePanel, theme.panelBg, theme.panelBorder);
    
    y += metrics.padding;
    
    if (_textRenderer) {
        _textRenderer->drawText("Tile (" + std::to_string(selectedX) + "," + std::to_string(selectedY) + ")", 
                               tilePanel.x + metrics.padding, y, theme.text);
    }
    y += metrics.textLineHeight;
    
    const auto* res = server.getTileResources(selectedX, selectedY);
    if (res) {
        const char* resNames[7] = {"Food", "Linemate", "Deraumere", "Sibur", "Mendiane", "Phiras", "Thystame"};
        
        for (int r = 0; r < 7; ++r) {
            if (res->qty[r] > 0) {
                std::string line = std::string(resNames[r]) + ": " + std::to_string(res->qty[r]);
                if (_textRenderer) {
                    _textRenderer->drawText(line, tilePanel.x + metrics.padding, y, theme.text);
                }
                y += metrics.textLineHeight;
            }
        }
    }
    
    const auto& players = server.getPlayers();
    bool headerDrawn = false;
    for (const auto& [id, p] : players) {
        if (p.x == selectedX && p.y == selectedY) {
            if (!headerDrawn) {
                if (_textRenderer) {
                    _textRenderer->drawText("Players:", tilePanel.x + metrics.padding, y, theme.text);
                }
                y += metrics.textLineHeight;
                headerDrawn = true;
            }
            
            auto it_player = theme.teamColors.find(p.team);
            SDL_Color col = (it_player != theme.teamColors.end()) ? it_player->second : theme.teamColors.at("DEFAULT");
            std::string label = "#" + std::to_string(id) + " (L" + std::to_string(p.level) + ")";
            
            if (_textRenderer) {
                _textRenderer->drawText(label, tilePanel.x + metrics.padding * 2, y, col);
            }
            y += metrics.textLineHeight;
        }
    }
    
    auto eggsOnTile = server.getEggsAt(selectedX, selectedY);
    if (!eggsOnTile.empty()) {
        y += metrics.padding / 2;
        if (_textRenderer) {
            _textRenderer->drawText("Eggs:", tilePanel.x + metrics.padding, y, theme.text);
        }
        y += metrics.textLineHeight;
        
        for (const auto* egg : eggsOnTile) {
            auto it = theme.teamColors.find(egg->team);
            SDL_Color eggCol = (it != theme.teamColors.end()) ? it->second : theme.teamColors.at("DEFAULT");
            std::string eggText = "Egg (" + egg->team + ")";
            
            if (_textRenderer) {
                _textRenderer->drawText(eggText, tilePanel.x + metrics.padding * 2, y, eggCol);
            }
            y += metrics.textLineHeight;
        }
    }
}

void SidebarRenderer::renderSelectedPlayerInfo(const LayoutMetrics& metrics, const ServerUpdateManager& server,
                                              int selectedPlayerId) {
    const auto& theme = GUI::ThemeManager::getTheme();
    const auto& players = server.getPlayers();
    bool hasPlayer = false;
    const ServerUpdateManager::Player* playerPtr = nullptr;

    if (selectedPlayerId >= 0) {
        auto it = players.find(selectedPlayerId);
        if (it != players.end()) {
            hasPlayer = true;
            playerPtr = &it->second;
        }
    }

    int minPanelHeight = metrics.textLineHeight * 6 + metrics.padding * 2 + 10;
    int desiredHeight = metrics.textLineHeight * 17 + metrics.padding * 2 + 10;
    int panelHeight = std::min(desiredHeight, static_cast<int>(metrics.sidebarHeight * 0.75f));
    if (panelHeight < minPanelHeight) panelHeight = minPanelHeight;
    int yStart = metrics.sidebarY + metrics.sidebarHeight - panelHeight - metrics.padding;
    
    SDL_Rect playerPanel{metrics.sidebarX + metrics.padding, yStart, 
                        metrics.sidebarWidth - metrics.padding * 2, panelHeight};
    drawPanel(playerPanel, theme.overlayBg, theme.overlayBorder);
    
    int y = playerPanel.y + metrics.padding;
    
    auto writeLine=[&](const std::string& txt, SDL_Color col){
        if (_textRenderer) _textRenderer->drawText(txt, playerPanel.x + metrics.padding, y, col);
        y += metrics.textLineHeight;
        return true;
    };
    
    if (!hasPlayer) {
        if (selectedPlayerId < 0)
            writeLine("No player selected", theme.textDisabled);
        else
            writeLine("Player disconnected", theme.textDisabled);
        return;
    }

    const auto& player = *playerPtr;

    if(!writeLine("Player #"+std::to_string(player.id)+" ("+player.team+")", theme.overlayAccent)) return;
    if(!writeLine("Level: "+std::to_string(player.level), theme.text)) return;
    
    if(!writeLine("Life:", theme.text)) return;
    float lifePct = player.lifeMeter>0? std::min(1.0f, player.lifeMeter/126.0f):1.0f;
    int barW = playerPanel.w - metrics.padding*2;
    int barH = 10;
    int barX = playerPanel.x + metrics.padding;
    int barY = y;
    if(barY + barH > playerPanel.y + playerPanel.h - metrics.padding) {
        writeLine(std::to_string(int(lifePct*100)) + "%", theme.accent);
    } else {
        drawProgressBar(barX, barY, barW, barH, lifePct, theme.accent, theme.panelBorder);
        y += barH + metrics.padding/2;
    }
    
    if(!writeLine("Inventory:",theme.textDisabled)) return;
    const char* resNames[7] = {"Food","Ln","De","Si","Me","Ph","Th"};
    for(int r=0;r<7;++r){
        std::string line=resNames[r]+std::string(": ")+std::to_string(player.inv.qty[r]);
        if(!writeLine(line,theme.text)) break;
    }
}

void SidebarRenderer::drawPanel(const SDL_Rect& rect, const SDL_Color& bgColor, const SDL_Color& borderColor) {
    // Background
    SDL_SetRenderDrawColor(_renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderFillRect(_renderer, &rect);
    
    // Border
    SDL_SetRenderDrawColor(_renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    SDL_RenderDrawRect(_renderer, &rect);
}

void SidebarRenderer::drawPanelNoBottom(const SDL_Rect& rect, const SDL_Color& bgColor, const SDL_Color& borderColor) {
    // Background
    SDL_SetRenderDrawColor(_renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderFillRect(_renderer, &rect);

    // Border (top, left, right)
    SDL_SetRenderDrawColor(_renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    // Top
    SDL_RenderDrawLine(_renderer, rect.x, rect.y, rect.x + rect.w, rect.y);
    // Left
    SDL_RenderDrawLine(_renderer, rect.x, rect.y, rect.x, rect.y + rect.h);
    // Right
    SDL_RenderDrawLine(_renderer, rect.x + rect.w, rect.y, rect.x + rect.w, rect.y + rect.h);
}

void SidebarRenderer::drawProgressBar(int x, int y, int width, int height, float percentage,
                                     const SDL_Color& fillColor, const SDL_Color& bgColor) {
    // Background
    SDL_SetRenderDrawColor(_renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_Rect bgRect{x, y, width, height};
    SDL_RenderFillRect(_renderer, &bgRect);
    
    // Fill
    if (percentage > 0) {
        SDL_SetRenderDrawColor(_renderer, fillColor.r, fillColor.g, fillColor.b, fillColor.a);
        SDL_Rect fillRect{x, y, static_cast<int>(width * percentage), height};
        SDL_RenderFillRect(_renderer, &fillRect);
    }
    
    // Border
    SDL_SetRenderDrawColor(_renderer, 100, 100, 100, 255);
    SDL_RenderDrawRect(_renderer, &bgRect);
}

void SidebarRenderer::drawResourceIcon(int x, int y, int size, int resourceType) {
    SDL_Rect iconRect{x, y, size, size};

    SDL_Texture* tex = nullptr;
    if (resourceType >= 0 && resourceType < 7) {
        if (_snoopMode && _snoopResourceTextures[resourceType]) {
            tex = _snoopResourceTextures[resourceType];
        } else {
            tex = _resourceTextures[resourceType];
        }
    }

    if (tex) {
        int w,h; SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);
        float scale = std::min(static_cast<float>(size)/w, static_cast<float>(size)/h);
        int dstW = static_cast<int>(w*scale);
        int dstH = static_cast<int>(h*scale);
        SDL_Rect dst{ x + (size-dstW)/2, y + (size-dstH)/2, dstW, dstH };
        SDL_RenderCopy(_renderer, tex, nullptr, &dst);
    } else {
        const SDL_Color resColors[7] = {
            {255, 165, 0, 255}, {255, 255, 255, 255}, {124, 252, 0, 255},
            {0, 255, 255, 255}, {255, 0, 255, 255}, {255, 255, 0, 255}, {148, 0, 211, 255}
        };
        SDL_SetRenderDrawColor(_renderer, resColors[resourceType].r, resColors[resourceType].g, resColors[resourceType].b, resColors[resourceType].a);
        SDL_RenderFillRect(_renderer, &iconRect);
    }
}

int SidebarRenderer::calculatePlayerListY(const LayoutMetrics& metrics) {
    return metrics.buttonHeight + metrics.padding * 2;
}

int SidebarRenderer::calculateResourceLegendY(const LayoutMetrics& metrics, const ServerUpdateManager& server) {
    int y = calculatePlayerListY(metrics);

    const auto& teamNames = server.getTeamNames();
    const auto& players   = server.getPlayers();

    bool summaryMode = metrics.isCompactMode;

    for (const auto& teamName : teamNames) {
        y += metrics.textLineHeight;

        if (!summaryMode) {
            int teamPlayerCount = 0;
            for (const auto& [id, p] : players) {
                if (p.team == teamName) teamPlayerCount++;
            }
            y += teamPlayerCount * metrics.textLineHeight;
        }

        y += metrics.padding;
    }

    y += metrics.padding;
    return y;
}

int SidebarRenderer::calculateSelectedTileY(const LayoutMetrics& metrics, const ServerUpdateManager& server) {
    int y = calculateResourceLegendY(metrics, server);
    y += metrics.textLineHeight;
    y += 7 * metrics.textLineHeight;
    y += metrics.padding * 2;
    
    return y;
}

void SidebarRenderer::updateClickableAreas(const LayoutMetrics& /*metrics*/,
                                          const ServerUpdateManager& /*server*/,
                                          std::vector<std::pair<SDL_Rect, int>>& /*clickablePlayerRects*/) {
}

int SidebarRenderer::getPlayerIdAt(int screenX, int screenY, const LayoutMetrics& /*metrics*/,
                                  const std::vector<std::pair<SDL_Rect, int>>& clickablePlayerRects) {
    for (const auto& [rect, playerId] : clickablePlayerRects) {
        if (screenX >= rect.x && screenX <= rect.x + rect.w &&
            screenY >= rect.y && screenY <= rect.y + rect.h) {
            return playerId;
        }
    }
    return -1;
}

bool SidebarRenderer::isTimerClicked(int screenX, int screenY) const {
    return (screenX >= _timerRect.x && screenX <= _timerRect.x + _timerRect.w &&
            screenY >= _timerRect.y && screenY <= _timerRect.y + _timerRect.h);
}

void SidebarRenderer::toggleTimerMode() {
    _showGameTime = !_showGameTime;
}

void SidebarRenderer::setGameStartTime(Uint32 startTime) {
    _gameStartTime = startTime;
}

// ------------------------------------------------------------------
// Scroll handling
// ------------------------------------------------------------------

void SidebarRenderer::scroll(int amount) {
    // Positive amount means moving content down (scrolling wheel down)
    _scrollOffset += amount;
    if (_scrollOffset < 0) _scrollOffset = 0;
    // NOTE: We deliberately do not clamp maximum offset; the clip rect will simply
    // render empty space if the user scrolls past the end of the content. 
}

// ------------------------------------------------------------------
// Helper: compute overlay rectangle without drawing
// ------------------------------------------------------------------

SDL_Rect SidebarRenderer::computePlayerPanelRect(const LayoutMetrics& metrics,
                                                const ServerUpdateManager& /*server*/,
                                                int selectedPlayerId) {
    int minPanelHeight = metrics.textLineHeight * 6 + metrics.padding * 2 + 10;

    int panelHeight;
    if (selectedPlayerId < 0) {
        panelHeight = minPanelHeight; // reserve minimal space when no player
    } else {
        int desiredHeight = metrics.textLineHeight * 17 + metrics.padding * 2 + 10;
        panelHeight = std::min(desiredHeight, static_cast<int>(metrics.sidebarHeight * 0.75f));
        if (panelHeight < minPanelHeight) panelHeight = minPanelHeight;
    }

    int yStart = metrics.sidebarY + metrics.sidebarHeight - panelHeight - metrics.padding;

    return {metrics.sidebarX + metrics.padding, yStart,
            metrics.sidebarWidth - metrics.padding * 2, panelHeight};
}

// ---------------------------------------------------------
// Resource sprite loading
// ---------------------------------------------------------

bool SidebarRenderer::loadResourceSprites() {
    const char* paths[7] = {
        "src/gui/assets/food.png",
        "src/gui/assets/gem1.png",
        "src/gui/assets/gem2.png",
        "src/gui/assets/gem3.png",
        "src/gui/assets/gem4.png",
        "src/gui/assets/gem5.png",
        "src/gui/assets/gem6.png"
    };
    bool ok = true;
    for (int i = 0; i < 7; ++i) {
        SDL_Surface* surf = IMG_Load(paths[i]);
        if (!surf) {
            std::cerr << "Sidebar: could not load " << paths[i] << ": " << IMG_GetError() << std::endl;
            _resourceTextures[i] = nullptr;
            ok = false;
            continue;
        }
        _resourceTextures[i] = SDL_CreateTextureFromSurface(_renderer, surf);
        SDL_FreeSurface(surf);
        if (!_resourceTextures[i]) {
            std::cerr << "Sidebar: could not create texture from " << paths[i] << ": " << SDL_GetError() << std::endl;
            ok = false;
        }
    }
    return ok;
}

void SidebarRenderer::setSnoopMode(bool snoopMode, SDL_Texture* snoopTextures[7]) {
    _snoopMode = snoopMode;
    if (snoopMode && snoopTextures) {
        for (int i = 0; i < 7; ++i) {
            _snoopResourceTextures[i] = snoopTextures[i];
        }
    }
} 
