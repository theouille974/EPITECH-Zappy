#include "LayoutManager.hpp"
#include <algorithm>
#include <cmath>

LayoutManager::LayoutManager() {
}

LayoutMetrics LayoutManager::calculateLayout(int windowWidth, int windowHeight, int boardWidth, int boardHeight) {
    LayoutMetrics metrics;
    
    metrics.windowWidth = windowWidth;
    metrics.windowHeight = windowHeight;
    
    metrics.isCompactMode = shouldUseCompactLayout(windowWidth, windowHeight);
    metrics.isWideMode = windowWidth >= WIDE_BREAKPOINT;
    
    float uiScale = getUIScale(windowWidth, windowHeight);
    
    metrics.padding = static_cast<int>(8 * uiScale);
    metrics.margin = static_cast<int>(12 * uiScale);
    metrics.buttonHeight = static_cast<int>(40 * uiScale);
    metrics.textLineHeight = static_cast<int>(18 * uiScale);
    
    metrics.sidebarWidth = getSidebarWidth(windowWidth);
    metrics.sidebarX = windowWidth - metrics.sidebarWidth;
    metrics.sidebarY = 0;
    metrics.sidebarHeight = windowHeight;
    
    metrics.boardAreaX = 0;
    metrics.boardAreaY = 0;
    metrics.boardAreaWidth = windowWidth - metrics.sidebarWidth;
    metrics.boardAreaHeight = windowHeight;
    
    if (boardWidth > 0 && boardHeight > 0) {
        metrics.cellWidth = static_cast<float>(metrics.boardAreaWidth) / boardWidth;
        metrics.cellHeight = static_cast<float>(metrics.boardAreaHeight) / boardHeight;
    } else {
        metrics.cellWidth = 0;
        metrics.cellHeight = 0;
    }
    
    return metrics;
}

int LayoutManager::getSidebarWidth(int windowWidth) {
    if (windowWidth < 600) {
        return std::max(MIN_SIDEBAR_WIDTH, windowWidth / 3);
    } else if (windowWidth < COMPACT_BREAKPOINT) {
        return std::max(MIN_SIDEBAR_WIDTH, windowWidth / 4);
    } else if (windowWidth >= WIDE_BREAKPOINT) {
        return std::min(MAX_SIDEBAR_WIDTH, windowWidth / 3);
    } else {
        return PREFERRED_SIDEBAR_WIDTH;
    }
}

SDL_Point LayoutManager::getMinimumWindowSize() {
    return {MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT};
}

bool LayoutManager::shouldUseCompactLayout(int windowWidth, int windowHeight) {
    return windowWidth < COMPACT_BREAKPOINT || windowHeight < 600 || windowWidth < 800;
}

float LayoutManager::getUIScale(int windowWidth, int windowHeight) {
    float baseScale = std::min(windowWidth / 1200.0f, windowHeight / 800.0f);
    float scale = std::clamp(baseScale, 0.6f, 1.5f);
    
    if (windowWidth < 800 || windowHeight < 600) {
        scale *= 0.8f;
    }
    
    return scale;
}

SDL_Point LayoutManager::boardToScreen(int boardX, int boardY, const LayoutMetrics& metrics) {
    return {
        static_cast<int>(metrics.boardAreaX + boardX * metrics.cellWidth),
        static_cast<int>(metrics.boardAreaY + boardY * metrics.cellHeight)
    };
}

SDL_Point LayoutManager::screenToBoard(int screenX, int screenY, const LayoutMetrics& metrics) {
    if (metrics.cellWidth <= 0 || metrics.cellHeight <= 0) {
        return {-1, -1};
    }
    
    int boardX = static_cast<int>((screenX - metrics.boardAreaX) / metrics.cellWidth);
    int boardY = static_cast<int>((screenY - metrics.boardAreaY) / metrics.cellHeight);
    
    return {boardX, boardY};
}

bool LayoutManager::isPointInBoardArea(int screenX, int screenY, const LayoutMetrics& metrics) {
    return screenX >= metrics.boardAreaX && 
           screenX < metrics.boardAreaX + metrics.boardAreaWidth &&
           screenY >= metrics.boardAreaY && 
           screenY < metrics.boardAreaY + metrics.boardAreaHeight;
}

bool LayoutManager::isPointInSidebarArea(int screenX, int screenY, const LayoutMetrics& metrics) {
    return screenX >= metrics.sidebarX && 
           screenX < metrics.sidebarX + metrics.sidebarWidth &&
           screenY >= metrics.sidebarY && 
           screenY < metrics.sidebarY + metrics.sidebarHeight;
} 