#include "TextRenderer.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>

TextRenderer::TextRenderer(SDL_Renderer* renderer, const std::string& fontPath, int fontSize)
    : _renderer(renderer) {
    if (TTF_WasInit() == 0) {
        if (TTF_Init() == -1) {
            std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;
            return;
        }
    }

    _font = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (!_font) {
        std::cerr << "Failed to load font " << fontPath << ": " << TTF_GetError() << std::endl;
    }
    
    _viewport = {0, 0, 1920, 1080};
}

TextRenderer::~TextRenderer() {
    clearCache();
    if (_font)
        TTF_CloseFont(_font);
    if (TTF_WasInit())
        TTF_Quit();
}

void TextRenderer::clearCache() {
    for (auto& [_, cached] : _textureCache) {
        if (cached.texture) SDL_DestroyTexture(cached.texture);
    }
    _textureCache.clear();
}

void TextRenderer::beginBatch() {
    _batching = true;
    _batchItems.clear();
}

void TextRenderer::endBatch() {
    _batching = false;
    flushBatch();
}

void TextRenderer::flushBatch() {
    if (_batchItems.empty()) return;
    
    std::sort(_batchItems.begin(), _batchItems.end(), 
        [](const BatchItem& a, const BatchItem& b) {
            return a.texture < b.texture;
        });
    
    for (const auto& item : _batchItems) {
        SDL_RenderCopy(_renderer, item.texture, &item.srcRect, &item.dstRect);
    }
    
    _batchItems.clear();
}

void TextRenderer::setViewport(int x, int y, int w, int h) {
    _viewport = {x, y, w, h};
    _viewportSet = true;
}

void TextRenderer::clearViewport() {
    _viewportSet = false;
}

bool TextRenderer::isInViewport(int x, int y, int w, int h) const {
    if (!_viewportSet) return true;
    return !(x + w < _viewport.x || x > _viewport.x + _viewport.w ||
             y + h < _viewport.y || y > _viewport.y + _viewport.h);
}

void TextRenderer::cleanupCache() {
    if (_textureCache.size() <= MAX_CACHE_SIZE) return;
    
    std::vector<std::pair<Uint32, std::string>> lru;
    for (const auto& [key, cached] : _textureCache) {
        lru.emplace_back(cached.lastUsed, key);
    }
    
    std::sort(lru.begin(), lru.end());
    
    size_t toRemove = _textureCache.size() - MAX_CACHE_SIZE * 8 / 10;
    for (size_t i = 0; i < toRemove && i < lru.size(); ++i) {
        auto it = _textureCache.find(lru[i].second);
        if (it != _textureCache.end()) {
            if (it->second.texture) SDL_DestroyTexture(it->second.texture);
            _textureCache.erase(it);
        }
    }
}

void TextRenderer::setScaleFactor(float scale) {
    _scaleFactor = scale;
    if (_scaleFactor < 0.5f) _scaleFactor = 0.5f;
    if (_scaleFactor > 2.0f) _scaleFactor = 2.0f;
}

void TextRenderer::drawText(const std::string& text, int x, int y, SDL_Color color) {
    if (!_font || text.empty())
        return;

    std::ostringstream key;
    key << text << "_" << (int)color.r << "_" << (int)color.g << "_" << (int)color.b << "_" << (int)color.a;
    std::string cacheKey = key.str();

    auto it = _textureCache.find(cacheKey);
    CachedTexture* cached = nullptr;
    
    if (it != _textureCache.end()) {
        cached = &it->second;
        cached->lastUsed = _frameCounter;
    } else {
        SDL_Surface* surface = TTF_RenderUTF8_Blended(_font, text.c_str(), color);
        if (!surface)
            return;

        if (surface->w == 0 || surface->h == 0) {
            SDL_FreeSurface(surface);
            return;
        }

        int surfW = surface->w;
        int surfH = surface->h;

        SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, surface);
        SDL_FreeSurface(surface);
        
        if (!texture) {
            return;
        }

        CachedTexture newCached;
        newCached.texture = texture;
        newCached.width = surfW;
        newCached.height = surfH;
        newCached.lastUsed = _frameCounter;
        
        auto [inserted, _] = _textureCache.emplace(cacheKey, std::move(newCached));
        cached = &inserted->second;
        
        cleanupCache();
    }

    if (!isInViewport(x, y, cached->width, cached->height)) {
        return;
    }

    if (_batching) {
        BatchItem item;
        item.texture = cached->texture;
        item.srcRect = {0, 0, cached->width, cached->height};
        item.dstRect = {x, y, static_cast<int>(cached->width * _scaleFactor), static_cast<int>(cached->height * _scaleFactor)};
        _batchItems.push_back(item);
    } else {
        SDL_Rect dstRect{x, y, static_cast<int>(cached->width * _scaleFactor), static_cast<int>(cached->height * _scaleFactor)};
        SDL_RenderCopy(_renderer, cached->texture, nullptr, &dstRect);
    }
    
    _frameCounter++;
} 