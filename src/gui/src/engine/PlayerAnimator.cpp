#include "PlayerAnimator.hpp"
#include <algorithm>
#include <cmath>
#include <SDL2/SDL_image.h>

namespace {
static float clamp01(float v) { return std::max(0.f, std::min(1.f, v)); }

static SDL_Color teamColor(const std::string& team) {
    if (team == "RED") return {255, 60, 60, 255};
    if (team == "BLUE") return {70, 130, 255, 255};
    if (team == "GREEN") return {0, 220, 100, 255};
    if (team == "YELLOW") return {255, 220, 0, 255};
    if (team == "SNOOP_DOG") return {0, 220, 100, 255};
    return {200, 200, 200, 255};
}
}

PlayerAnimator::PlayerAnimator(SDL_Renderer* renderer, TweenSystem& tween)
    : _renderer(renderer), _tween(tween) {
    SDL_Surface* surf = IMG_Load("src/gui/assets/snoop.png");
    if (surf) {
        _snoopTexture = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
        if (_snoopTexture) {
            SDL_SetTextureBlendMode(_snoopTexture, SDL_BLENDMODE_BLEND);
        }
    }
}

PlayerAnimator::~PlayerAnimator() {
    if (_snoopTexture) SDL_DestroyTexture(_snoopTexture);
}

void PlayerAnimator::syncWithServer(const ServerUpdateManager& server) {
    std::unordered_map<int, bool> seen;
    seen.reserve(_visuals.size());
    for (auto& [id, v] : _visuals) seen[id] = false;

    const auto& players = server.getPlayers();
    for (const auto& [id, p] : players) {
        seen[id] = true;
        auto it = _visuals.find(id);
        if (it == _visuals.end()) {
            Visual v;
            v.x = static_cast<float>(p.x);
            v.y = static_cast<float>(p.y);
            v.orientation = p.orientation;
            v.level = p.level;
            v.team = p.team;
            v.lifePercentage = p.lifeMeter > 0 ? clamp01(p.lifeMeter / 126.f) : 1.f;
            _visuals[id] = v;
            continue;
        }
        Visual& vis = it->second;
        if (static_cast<int>(roundf(vis.x)) != p.x || static_cast<int>(roundf(vis.y)) != p.y) {
            _tween.add(&vis.x, static_cast<float>(p.x), 0.25f);
            _tween.add(&vis.y, static_cast<float>(p.y), 0.25f);
        }
        vis.orientation = p.orientation;
        vis.level = p.level;
        vis.team = p.team;
        vis.lifePercentage = p.lifeMeter > 0 ? clamp01(p.lifeMeter / 126.f) : 1.f;
    }

    for (auto it = _visuals.begin(); it != _visuals.end();) {
        if (!seen[it->first]) {
            _tween.removeTarget(&it->second.x);
            _tween.removeTarget(&it->second.y);
            it = _visuals.erase(it);
        } else {
            ++it;
        }
    }
}

void PlayerAnimator::render(int viewportWidth, int viewportHeight,
                            int boardW, int boardH,
                            int selectedPlayerId,
                            TextRenderer* text) {
    if (boardW <= 0 || boardH <= 0) return;

    float cellW = static_cast<float>(viewportWidth) / boardW;
    float cellH = static_cast<float>(viewportHeight) / boardH;

    for (const auto& [id, v] : _visuals) {
        float px = v.x * cellW + cellW / 2.f;
        float py = v.y * cellH + cellH / 2.f;
        int radius = static_cast<int>(std::min(cellW, cellH) / 3.f);
        if (radius < 4) radius = 4;
        SDL_Color col = teamColor(v.team);
        bool highlight = (id == selectedPlayerId);
        drawPlayerPixel(px, py, radius, col, v.lifePercentage, id, v.orientation, text, highlight, v.team);
    }
}

void PlayerAnimator::drawPlayerPixel(float px, float py, int radius, const SDL_Color& color,
                                     float lifePercentage, int playerId, int orientation,
                                     TextRenderer* text, bool highlight, const std::string& team) {
    SDL_Rect rect{static_cast<int>(px - radius), static_cast<int>(py - radius), radius * 2, radius * 2};

    if (team == "SNOOP_DOG" && _snoopTexture) {
        int texW = 0, texH = 0;
        SDL_QueryTexture(_snoopTexture, nullptr, nullptr, &texW, &texH);
        float aspect = (texH != 0) ? static_cast<float>(texW) / texH : 1.f;
        float scale = 1.3f;
        int destW = static_cast<int>(radius * 2 * scale);
        int maxSize = radius * 3;
        if (destW > maxSize) destW = maxSize;
        int destH = static_cast<int>(destW / aspect);
        if (destH > maxSize) { destH = maxSize; destW = static_cast<int>(destH * aspect);}
        SDL_Rect dst{static_cast<int>(px - destW / 2), static_cast<int>(py - destH / 2), destW, destH};
        SDL_RenderCopy(_renderer, _snoopTexture, nullptr, &dst);
    } else {
        SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(_renderer, &rect);
    }

    int lifeBarWidth = radius * 2;
    int lifeBarHeight = 6;
    int lifeBarX = rect.x;
    int lifeBarY = rect.y - lifeBarHeight - 2;

    SDL_SetRenderDrawColor(_renderer, 80, 20, 20, 255);
    SDL_Rect bg{lifeBarX, lifeBarY, lifeBarWidth, lifeBarHeight};
    SDL_RenderFillRect(_renderer, &bg);

    int lifeWidth = static_cast<int>(lifeBarWidth * clamp01(lifePercentage));
    SDL_Color lifeCol{0, 200, 0, 255};
    if (lifePercentage <= 0.3f) lifeCol = {200, 0, 0, 255};
    else if (lifePercentage <= 0.6f) lifeCol = {200, 200, 0, 255};

    SDL_SetRenderDrawColor(_renderer, lifeCol.r, lifeCol.g, lifeCol.b, lifeCol.a);
    SDL_Rect lifeRect{lifeBarX, lifeBarY, lifeWidth, lifeBarHeight};
    SDL_RenderFillRect(_renderer, &lifeRect);

    SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(_renderer, &bg);

    int indRadius = std::max(2, radius / 3);
    int offsetX = 0, offsetY = 0;
    switch (orientation) {
        case 1: offsetY = -radius - indRadius; break; // North
        case 2: offsetX =  radius + indRadius; break; // East
        case 3: offsetY =  radius + indRadius; break; // South
        case 4: offsetX = -radius - indRadius; break; // West
        default: break;
    }
    SDL_Rect ind{static_cast<int>(px + offsetX - indRadius), static_cast<int>(py + offsetY - indRadius), indRadius*2, indRadius*2};
    SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(_renderer, &ind);
    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(_renderer, &ind);

    if (text) {
        std::string idStr = std::to_string(playerId);

        float prevScale = text->getScaleFactor();

        float estWidth = static_cast<float>(idStr.size()) * 8.0f * prevScale;

        float maxWidth = static_cast<float>(radius * 2);
        float newScale = prevScale;
        if (estWidth > maxWidth && estWidth > 0.0f) {
            newScale = prevScale * (maxWidth / estWidth);
        }

        text->setScaleFactor(newScale);

        float charW = 8.0f * newScale;
        float textW = static_cast<float>(idStr.size()) * charW;
        float textH = 8.0f * newScale;

        int textX = static_cast<int>(px - textW / 2.0f);
        int textY = static_cast<int>(py - textH / 2.0f - radius * 0.2f);

        SDL_Color txtCol = (color.r + color.g + color.b > 500) ? SDL_Color{0, 0, 0, 255}
                                                              : SDL_Color{255, 255, 255, 255};

        text->drawText(idStr, textX, textY, txtCol);

        text->setScaleFactor(prevScale);
    }

    if (highlight) {
        SDL_SetRenderDrawColor(_renderer, 255, 255, 0, 255);
        SDL_Rect outline{rect.x - 2, rect.y - 2, rect.w + 4, rect.h + 4};
        SDL_RenderDrawRect(_renderer, &outline);
    }
} 