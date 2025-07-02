#include "BoardRenderer.hpp"
#include "../core/ServerUpdateManager.hpp"
#include "../core/Theme.hpp"
#include <string>
#include <iostream>

BoardRenderer::BoardRenderer(SDL_Renderer* renderer)
    : _renderer(renderer) {
    if (!loadEggSprite()) {
        std::cerr << "Warning: Could not load egg sprite, using fallback rendering" << std::endl;
    }

    if (!loadResourceSprites()) {
        std::cerr << "Warning: Could not load some resource sprites, using fallback colored squares" << std::endl;
    }
}

BoardRenderer::~BoardRenderer() {
    if (_eggTexture) {
        SDL_DestroyTexture(_eggTexture);
        _eggTexture = nullptr;
    }
    
    for (auto& [_, texture] : _teamEggTextures) {
        if (texture) {
            SDL_DestroyTexture(texture);
        }
    }
    _teamEggTextures.clear();

    for (SDL_Texture*& tex : _resourceTextures) {
        if (tex) {
            SDL_DestroyTexture(tex);
            tex = nullptr;
        }
    }
}

void BoardRenderer::setBoardSize(int width, int height) {
    _boardWidth = width;
    _boardHeight = height;
}

void BoardRenderer::setSnoopMode(bool snoopMode, SDL_Texture* snoopTextures[7]) {
    _snoopMode = snoopMode;
    if (snoopMode && snoopTextures) {
        for (int i = 0; i < 7; ++i) {
            _snoopResourceTextures[i] = snoopTextures[i];
        }
    }
}

bool BoardRenderer::loadEggSprite() {
    SDL_Surface* surface = IMG_Load("src/gui/assets/egg_sprite.png");
    if (!surface) {
        std::cerr << "Could not load egg_sprite.png: " << IMG_GetError() << std::endl;
        return false;
    }
    
    _eggTexture = SDL_CreateTextureFromSurface(_renderer, surface);
    SDL_FreeSurface(surface);
    
    if (!_eggTexture) {
        std::cerr << "Could not create texture from egg sprite: " << SDL_GetError() << std::endl;
        return false;
    }
    
    return true;
}

SDL_Texture* BoardRenderer::getTeamEggTexture(const std::string& team) {
    auto it = _teamEggTextures.find(team);
    if (it != _teamEggTextures.end()) {
        return it->second;
    }
    
    if (!_eggTexture) return nullptr;
    
    int w, h;
    SDL_QueryTexture(_eggTexture, nullptr, nullptr, &w, &h);
    
    SDL_Texture* newTexture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, 
                                               SDL_TEXTUREACCESS_TARGET, w, h);
    if (!newTexture) return nullptr;
    
    SDL_SetRenderTarget(_renderer, newTexture);
    SDL_RenderCopy(_renderer, _eggTexture, nullptr, nullptr);
    SDL_SetRenderTarget(_renderer, nullptr);
    
    if (team == "RED") {
        SDL_SetTextureColorMod(newTexture, 255, 100, 100);
    } else if (team == "BLUE") {
        SDL_SetTextureColorMod(newTexture, 100, 150, 255);
    } else if (team == "GREEN") {
        SDL_SetTextureColorMod(newTexture, 100, 255, 150);
    } else if (team == "YELLOW") {
        SDL_SetTextureColorMod(newTexture, 255, 255, 100);
    } else {
        SDL_SetTextureColorMod(newTexture, 255, 255, 255);
    }
    
    _teamEggTextures[team] = newTexture;
    return newTexture;
}

void BoardRenderer::setTeamColorMod(const std::string& team) {
    if (_lastTeamColorMod == team) return;
    
    if (team == "RED") {
        SDL_SetTextureColorMod(_eggTexture, 255, 100, 100);
    } else if (team == "BLUE") {
        SDL_SetTextureColorMod(_eggTexture, 100, 150, 255);
    } else if (team == "GREEN") {
        SDL_SetTextureColorMod(_eggTexture, 100, 255, 150);
    } else if (team == "YELLOW") {
        SDL_SetTextureColorMod(_eggTexture, 255, 255, 100);
    } else {
        SDL_SetTextureColorMod(_eggTexture, 255, 255, 255);
    }
    
    _lastTeamColorMod = team;
}

void BoardRenderer::render(const LayoutMetrics& metrics,
                          std::function<const ServerUpdateManager::Resources*(int, int)> getResources,
                          std::function<std::vector<const ServerUpdateManager::Egg*>(int, int)> getEggs,
                          int selectedX, int selectedY) {
    if (_boardWidth <= 0 || _boardHeight <= 0) return;

    for (int y = 0; y < _boardHeight; ++y) {
        for (int x = 0; x < _boardWidth; ++x) {
            bool isSelected = (x == selectedX && y == selectedY);
            
            drawCellBackground(x, y, metrics.cellWidth, metrics.cellHeight, isSelected);
            
            if (_showCellBorders) {
                drawCellBorder(x, y, metrics.cellWidth, metrics.cellHeight);
            }

            const auto* resources = getResources(x, y);
            if (resources && _showResourceIcons) {
                drawResources(x, y, metrics.cellWidth, metrics.cellHeight, resources);
            }

            auto eggs = getEggs(x, y);
            for (const auto* egg : eggs) {
                if (egg && egg->alive) {
                    drawEgg(x, y, metrics.cellWidth, metrics.cellHeight, egg->team, isSelected);
                }
            }
        }
    }
}

void BoardRenderer::render(int viewportWidth, int viewportHeight,
                          std::function<const ServerUpdateManager::Resources*(int, int)> getResources,
                          std::function<std::vector<const ServerUpdateManager::Egg*>(int, int)> getEggs,
                          int selectedX, int selectedY) {
    if (_boardWidth <= 0 || _boardHeight <= 0) return;

    float cellW = static_cast<float>(viewportWidth) / _boardWidth;
    float cellH = static_cast<float>(viewportHeight) / _boardHeight;

    // Draw grid and resources
    for (int y = 0; y < _boardHeight; ++y) {
        for (int x = 0; x < _boardWidth; ++x) {
            float pixelX = x * cellW;
            float pixelY = y * cellH;
            
            // Draw cell background
            bool isSelected = (x == selectedX && y == selectedY);
            SDL_Color acc = GUI::ThemeManager::getTheme().accent;
            SDL_Color bgColor = isSelected
                ? SDL_Color{acc.r, acc.g, acc.b, 180}
                : SDL_Color{acc.r, acc.g, acc.b, 90};
            SDL_SetRenderDrawColor(_renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
            SDL_Rect cellRect{static_cast<int>(pixelX), static_cast<int>(pixelY), 
                             static_cast<int>(cellW), static_cast<int>(cellH)};
            SDL_RenderFillRect(_renderer, &cellRect);

            // Draw cell border
            SDL_Color accBorder = GUI::ThemeManager::getTheme().accent;
            SDL_SetRenderDrawColor(_renderer, accBorder.r, accBorder.g, accBorder.b, 200);
            SDL_RenderDrawRect(_renderer, &cellRect);

            // Draw resources
            const auto* resources = getResources(x, y);
            if (resources) {
                const SDL_Color resColors[7] = {
                    {255,165,0,255}, {255,255,255,255}, {124,252,0,255}, 
                    {0,255,255,255}, {255,0,255,255}, {255,255,0,255}, {148,0,211,255}
                };
                
                int resIndex = 0;
                for (int r = 0; r < 7; ++r) {
                    if (resources->qty[r] > 0) {
                        int resX = static_cast<int>(pixelX + 5 + (resIndex % 3) * 8);
                        int resY = static_cast<int>(pixelY + 5 + (resIndex / 3) * 8);
                        
                        SDL_SetRenderDrawColor(_renderer, resColors[r].r, resColors[r].g, resColors[r].b, 255);
                        SDL_Rect resRect{resX, resY, 6, 6};
                        SDL_RenderFillRect(_renderer, &resRect);
                        
                        resIndex++;
                        if (resIndex >= 6) break;
                    }
                }
            }

            // Draw eggs
            auto eggs = getEggs(x, y);
            for (const auto* egg : eggs) {
                if (egg && egg->alive) {
                    drawEgg(x, y, cellW, cellH, egg->team, isSelected);
                }
            }
        }
    }
}

void BoardRenderer::drawCellBackground(int x, int y, float cellW, float cellH, bool isSelected) {
    SDL_Rect cellRect = getCellRect(x, y, cellW, cellH);
    
    SDL_Color acc = GUI::ThemeManager::getTheme().accent;
    SDL_Color bgColor = isSelected
        ? SDL_Color{acc.r, acc.g, acc.b, 180}
        : SDL_Color{acc.r, acc.g, acc.b, 90};
    SDL_SetRenderDrawColor(_renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderFillRect(_renderer, &cellRect);
}

void BoardRenderer::drawCellBorder(int x, int y, float cellW, float cellH) {
    SDL_Rect cellRect = getCellRect(x, y, cellW, cellH);
    
    SDL_Color accBorder = GUI::ThemeManager::getTheme().accent;
    SDL_SetRenderDrawColor(_renderer, accBorder.r, accBorder.g, accBorder.b, 200);
    SDL_RenderDrawRect(_renderer, &cellRect);
}

void BoardRenderer::drawResources(int x, int y, float cellW, float cellH, const ServerUpdateManager::Resources* resources) {
    if (!resources) return;
    
    SDL_Rect cellRect = getCellRect(x, y, cellW, cellH);
    int iconSize = static_cast<int>(std::min(cellW, cellH) * 0.20f);
    if (iconSize < 4) iconSize = 4;
    
    int resIndex = 0;
    int maxResources = static_cast<int>((cellRect.w * cellRect.h) / (iconSize * iconSize * 4));
    maxResources = std::min(maxResources, 6);
    
    for (int r = 0; r < 7 && resIndex < maxResources; ++r) {
        if (resources->qty[r] > 0) {
            int resX = cellRect.x + 2 + (resIndex % 3) * (iconSize + 2);
            int resY = cellRect.y + 2 + (resIndex / 3) * (iconSize + 2);
            
            drawResourceIcon(resX, resY, iconSize, r, resources->qty[r]);
            resIndex++;
        }
    }
}

bool BoardRenderer::loadResourceSprites() {
    const char* paths[7] = {
        "src/gui/assets/food.png",
        "src/gui/assets/gem1.png",
        "src/gui/assets/gem2.png",
        "src/gui/assets/gem3.png",
        "src/gui/assets/gem4.png",
        "src/gui/assets/gem5.png",
        "src/gui/assets/gem6.png"
    };
    bool allOk = true;
    for (int i = 0; i < 7; ++i) {
        SDL_Surface* surf = IMG_Load(paths[i]);
        if (!surf) {
            std::cerr << "Could not load " << paths[i] << ": " << IMG_GetError() << std::endl;
            _resourceTextures[i] = nullptr;
            allOk = false;
            continue;
        }
        _resourceTextures[i] = SDL_CreateTextureFromSurface(_renderer, surf);
        SDL_FreeSurface(surf);
        if (!_resourceTextures[i]) {
            std::cerr << "Could not create texture from " << paths[i] << ": " << SDL_GetError() << std::endl;
            allOk = false;
        }
    }
    return allOk;
}

void BoardRenderer::drawResourceIcon(int x, int y, int size, int resourceType, int quantity) {
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
        int w, h;
        SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);
        float scale = std::min(static_cast<float>(size) / w, static_cast<float>(size) / h);
        int dstW = static_cast<int>(w * scale);
        int dstH = static_cast<int>(h * scale);
        SDL_Rect dst { x + (size - dstW)/2, y + (size - dstH)/2, dstW, dstH };
        SDL_RenderCopy(_renderer, tex, nullptr, &dst);
    } else {
        const SDL_Color resColors[7] = {
            {255, 165, 0, 255}, {255, 255, 255, 255}, {124, 252, 0, 255}, 
            {0, 255, 255, 255}, {255, 0, 255, 255}, {255, 255, 0, 255}, {148, 0, 211, 255}
        };
        SDL_SetRenderDrawColor(_renderer, resColors[resourceType].r, resColors[resourceType].g, resColors[resourceType].b, resColors[resourceType].a);
        SDL_RenderFillRect(_renderer, &iconRect);
    }

    if (quantity > 1 && _textRenderer) {
        std::string qtyText = std::to_string(quantity);

        float prevScale = _textRenderer->getScaleFactor();
        float smallScale = prevScale * 0.3f;
        if (smallScale < 0.15f) smallScale = 0.15f;
        _textRenderer->setScaleFactor(smallScale);

        _textRenderer->drawText(qtyText, iconRect.x, iconRect.y, {255, 255, 255, 255});

        _textRenderer->setScaleFactor(prevScale);
    }
}

SDL_Rect BoardRenderer::getCellRect(int x, int y, float cellW, float cellH) {
    int paddingX = static_cast<int>(cellW * _cellPadding);
    int paddingY = static_cast<int>(cellH * _cellPadding);
    
    return {
        static_cast<int>(x * cellW) + paddingX,
        static_cast<int>(y * cellH) + paddingY,
        static_cast<int>(cellW) - paddingX * 2,
        static_cast<int>(cellH) - paddingY * 2
    };
}

void BoardRenderer::drawEgg(int x, int y, float cellW, float cellH, const std::string& team, bool isSelected) {
    int centerX = static_cast<int>(x * cellW + cellW/2);
    int centerY = static_cast<int>(y * cellH + cellH/2);
    
    int eggWidth = static_cast<int>(std::min(cellW, cellH) / 3);
    int eggHeight = static_cast<int>(eggWidth * 1.2f);
    
    if (eggWidth < 8) {
        eggWidth = 8;
        eggHeight = 10;
    }
    
    SDL_Rect destRect{
        centerX - eggWidth/2, 
        centerY - eggHeight/2, 
        eggWidth, 
        eggHeight
    };
    
    SDL_Texture* textureToUse = getTeamEggTexture(team);
    if (!textureToUse) {
        textureToUse = _eggTexture;
        if (textureToUse) {
            setTeamColorMod(team);
        }
    }
    
    if (textureToUse) {
        SDL_RenderCopy(_renderer, textureToUse, nullptr, &destRect);
    } else {
        SDL_Color eggColor{150, 150, 150, 255};
        
        if (team == "RED") eggColor = {200, 40, 40, 255};
        else if (team == "BLUE") eggColor = {50, 100, 200, 255};
        else if (team == "GREEN") eggColor = {0, 160, 80, 255};
        else if (team == "YELLOW") eggColor = {200, 180, 0, 255};
        
        SDL_SetRenderDrawColor(_renderer, eggColor.r, eggColor.g, eggColor.b, 255);
        SDL_RenderFillRect(_renderer, &destRect);
        
        SDL_SetRenderDrawColor(_renderer, 
                              static_cast<Uint8>(eggColor.r * 0.7f), 
                              static_cast<Uint8>(eggColor.g * 0.7f), 
                              static_cast<Uint8>(eggColor.b * 0.7f), 255);
        SDL_RenderDrawRect(_renderer, &destRect);
    }
    
    if (isSelected) {
        SDL_SetRenderDrawColor(_renderer, 255, 255, 0, 255);
        SDL_Rect selection{
            destRect.x - 2, 
            destRect.y - 2, 
            destRect.w + 4, 
            destRect.h + 4
        };
        SDL_RenderDrawRect(_renderer, &selection);
        
        SDL_Rect selection2{
            destRect.x - 1, 
            destRect.y - 1, 
            destRect.w + 2, 
            destRect.h + 2
        };
        SDL_RenderDrawRect(_renderer, &selection2);
    }
}

void BoardRenderer::setTextRenderer(TextRenderer* textRenderer) {
    _textRenderer = textRenderer;
}