#include "Effects.hpp"
#include <iostream>
#include <cmath>

EffectSystem::Particle* EffectSystem::getParticleFromPool() {
    for (auto* particle : _particlePool) {
        if (!particle->active) {
            return particle;
        }
    }
    
    Particle* newParticle = new Particle();
    _particlePool.push_back(newParticle);
    return newParticle;
}

void EffectSystem::returnParticleToPool(Particle* p) {
    p->active = false;
}

void EffectSystem::createParticleTexture() {
    const int size = 8;
    SDL_Surface* surface = SDL_CreateRGBSurface(0, size, size, 32, 0, 0, 0, 0);
    if (!surface) {
        std::cerr << "Failed to create particle surface" << std::endl;
        return;
    }
    
    SDL_FillRect(surface, nullptr, SDL_MapRGBA(surface->format, 0, 0, 0, 0));
    
    Uint32 white = SDL_MapRGBA(surface->format, 255, 255, 255, 255);
    int center = size / 2;
    int radius = size / 2 - 1;
    
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            int dx = x - center;
            int dy = y - center;
            if (dx * dx + dy * dy <= radius * radius) {
                Uint32* pixels = static_cast<Uint32*>(surface->pixels);
                pixels[y * size + x] = white;
            }
        }
    }
    
    _particleTexture = SDL_CreateTextureFromSurface(_renderer, surface);
    SDL_FreeSurface(surface);
    
    if (!_particleTexture) {
        std::cerr << "Failed to create particle texture" << std::endl;
    }
}

void EffectSystem::renderParticlesWithTexture(float cellW, float cellH, bool topLayer) {
    for (const auto& burst : _bursts) {
        if (burst.top != topLayer) continue;
        float baseX = burst.tileX * cellW + cellW / 2.f;
        float baseY = burst.tileY * cellH + cellH / 2.f;
        for (auto* p : burst.particles) {
            if (!p->active) continue;
            float t = p->elapsed / p->life;
            if (t > 1.f) continue;
            float px = baseX + p->dx * t;
            float py = baseY + p->dy * t;
            Uint8 alpha = static_cast<Uint8>((1 - t) * 255);
            SDL_SetTextureColorMod(_particleTexture, p->color.r, p->color.g, p->color.b);
            SDL_SetTextureAlphaMod(_particleTexture, alpha);
            int size = static_cast<int>(p->size);
            SDL_Rect dst{static_cast<int>(px - size/2),static_cast<int>(py - size/2),size,size};
            SDL_RenderCopy(_renderer, _particleTexture, nullptr, &dst);
        }
    }
    SDL_SetTextureColorMod(_particleTexture,255,255,255);
    SDL_SetTextureAlphaMod(_particleTexture,255);
}

void EffectSystem::renderParticlesAsRectangles(float cellW, float cellH, bool topLayer) {
    for (const auto& burst : _bursts) {
        if (burst.top != topLayer) continue;
        float baseX = burst.tileX * cellW + cellW / 2.f;
        float baseY = burst.tileY * cellH + cellH / 2.f;
        for (auto* p : burst.particles) {
            if (!p->active) continue;
            float t = p->elapsed / p->life;
            if (t > 1.f) continue;
            float px = baseX + p->dx * t;
            float py = baseY + p->dy * t;
            Uint8 alpha = static_cast<Uint8>((1 - t) * 255);
            SDL_SetRenderDrawColor(_renderer, p->color.r, p->color.g, p->color.b, alpha);
            SDL_Rect r{static_cast<int>(px - p->size/2),static_cast<int>(py - p->size/2),static_cast<int>(p->size),static_cast<int>(p->size)};
            SDL_RenderFillRect(_renderer,&r);
        }
    }
}

static void drawCircle(SDL_Renderer* renderer, int cx, int cy, int radius, const SDL_Color& col, Uint8 alpha)
{
    SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, alpha);
    const int step = 4;
    for (int deg = 0; deg < 360; deg += step) {
        float rad = static_cast<float>(deg) * static_cast<float>(M_PI) / 180.0f;
        int x = cx + static_cast<int>(std::cos(rad) * radius);
        int y = cy + static_cast<int>(std::sin(rad) * radius);
        SDL_RenderDrawPoint(renderer, x, y);
    }
}

void EffectSystem::updateRings(float dt)
{
    for (auto it = _rings.begin(); it != _rings.end();) {
        it->elapsed += dt;
        if (it->elapsed >= it->duration) {
            it = _rings.erase(it);
        } else {
            ++it;
        }
    }
}

void EffectSystem::renderRings(float cellW, float cellH, bool topLayer)
{
    float cellSize = std::min(cellW, cellH);
    for (const auto &ring : _rings) {
        if (ring.top != topLayer) continue;
        float t = ring.elapsed / ring.duration;
        float radiusPx = cellSize * ring.radiusMult * t;
        if (radiusPx <= 0.f) continue;
        float cx = ring.tileX * cellW + cellW / 2.f;
        float cy = ring.tileY * cellH + cellH / 2.f;
        Uint8 alpha = static_cast<Uint8>((1.0f - t) * 255);
        drawCircle(_renderer, static_cast<int>(cx), static_cast<int>(cy), static_cast<int>(radiusPx), ring.color, alpha);
    }
} 