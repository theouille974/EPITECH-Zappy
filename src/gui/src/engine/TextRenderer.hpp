/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** TextRenderer
*/

#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <unordered_map>
#include <vector>

class TextRenderer {
    public:
        // Constructors and Destructors
        /**
         * @brief Constructs a TextRenderer with the specified font and size.
         * @param renderer The SDL_Renderer to use for rendering text.
         * @param fontPath The file path to the TTF font file.
         * @param fontSize The size of the font in points.
         * @note This constructor initializes the text renderer, loads the font, and prepares it for
         * @note rendering text. It also sets up a texture cache to optimize text rendering performance.
         * @note The font must be a valid TTF file, and the renderer must be
         */
        TextRenderer(SDL_Renderer* renderer, const std::string& fontPath, int fontSize);
        /**
         * @brief Destructor that cleans up resources used by TextRenderer.
         * This destructor frees the loaded font and clears the texture cache to prevent memory leaks.
         */
        ~TextRenderer();

        // Public Rendering Methods
        /**
         * @brief Draws text at the specified coordinates with the given color.
         * @param text The string of text to draw.
         * @param x The X coordinate for the top-left corner of the text.
         * @param y The Y coordinate for the top-left corner of the text.
         * @param color The SDL_Color to use for rendering the text.
         * This method caches the texture for later use and manages memory efficiently.
         */
        void drawText(const std::string& text, int x, int y, SDL_Color color);

        // Batch Rendering Methods
        /**
         * @brief Begins a batch of rendering operations.
         * @note This method prepares the renderer for batching multiple draw calls into a single
         * @note operation, which can improve performance by reducing the number of draw calls.
         */
        void beginBatch();
        /**
         * @brief Ends the current batch of rendering operations.
         * @note This method finalizes the batch and submits all batched draw calls to the
         * @note renderer in a single operation, which can significantly improve rendering performance.
         * @note It should be called after all draw calls for the batch have been made.
         */
        void endBatch();
        /**
         * @brief Flushes the current batch of rendering operations.
         * @note This method submits the batched draw calls to the renderer immediately,
         * @note ensuring that all pending operations are executed. It is useful for cases where
         * @note immediate rendering is required, such as when switching between different rendering contexts.
         */
        void flushBatch();

        // Viewport Methods
        /**
         * @brief Sets the viewport for rendering.
         * @param x The X coordinate of the viewport's top-left corner.
         * @param y The Y coordinate of the viewport's top-left corner.
         * @param w The width of the viewport.
         * @param h The height of the viewport.
         * @note This method defines the area of the rendering target that will be used for drawing.
         * @note It allows for rendering to a specific portion of the screen or a texture.
         */
        void setViewport(int x, int y, int w, int h);
        /**
         * @brief Clears the current viewport settings.
         * @note This method resets the viewport to its default state, which is the entire rendering target.
         * @note It is useful for restoring the original rendering area after setting a custom viewport.
         */
        void clearViewport();

        // Scale Factor Methods
        /**
         * @brief Sets the scale factor for rendering.
         * @param scale The scale factor to apply to all rendered text.
         * @note This method adjusts the size of the rendered text based on the specified scale factor.
         * @note It allows for dynamic scaling of text, which can be useful for responsive designs or
         * @note adapting to different screen resolutions.
         */
        void setScaleFactor(float scale);
        /**
         * @brief Gets the current scale factor for rendering.
         * @return The current scale factor applied to rendered text.
         * @note This method retrieves the scale factor that is currently being used for rendering text.
         * @note It can be useful for debugging or adjusting rendering parameters dynamically.
         */
        float getScaleFactor() const { return _scaleFactor; }

    private:
        // Private Structs
        struct CachedTexture {
            SDL_Texture* texture;
            int width;
            int height;
            Uint32 lastUsed;
        };

        struct BatchItem {
            SDL_Texture* texture;
            SDL_Rect srcRect;
            SDL_Rect dstRect;
        };

        // Private Member Variables
        SDL_Renderer* _renderer;
        TTF_Font* _font;
        std::unordered_map<std::string, CachedTexture> _textureCache;

        std::vector<BatchItem> _batchItems;
        bool _batching = false;

        SDL_Rect _viewport;
        bool _viewportSet = false;

        static constexpr size_t MAX_CACHE_SIZE = 1000;
        Uint32 _frameCounter = 0;

        float _scaleFactor = 1.0f;

        // Private Helper Methods
        /**
         * @brief Clears the texture cache, removing all cached textures.
         * @note This method releases all textures stored in the cache, freeing up memory.
         * @note It is useful for managing memory usage, especially when textures are no longer needed.
         * @note After calling this method, all previously cached textures will be unloaded,
         */
        void clearCache();
        /**
         * @brief Checks if a rectangle is within the viewport.
         * @param x The X coordinate of the rectangle's top-left corner.
         * @param y The Y coordinate of the rectangle's top-left corner.
         * @param w The width of the rectangle.
         * @param h The height of the rectangle.
         * @return True if the rectangle is within the viewport, false otherwise.
         * @note This method is used to determine if a given area should be rendered based on
         * @note its position relative to the current viewport settings.
         */
        bool isInViewport(int x, int y, int w, int h) const;
        /**
         * @brief Cleans up the texture cache by removing unused textures.
         * @note This method iterates through the cached textures and removes those that have not
         * @note been used for a certain period, helping to manage memory usage efficiently.
         * @note It is called periodically to ensure that the cache does not grow indefinitely.
         */
        void cleanupCache();
};
