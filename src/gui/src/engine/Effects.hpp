/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** Effects
*/

#pragma once

#include <SDL2/SDL.h>
#include <cmath> // For std::cos, std::sin, M_PI
#include <iostream>
#include <memory>
#include <random> // For std::mt19937, std::random_device, std::uniform_real_distribution
#include <vector>

class EffectSystem {
    public:
        // --- Constructor and Destructor ---

        /**
         * @brief Constructs an EffectSystem with the given SDL_Renderer.
         * @param renderer The SDL_Renderer to use for rendering effects.
         * This constructor initializes the effect system, pre-allocates memory for
         * particles, and creates a simple particle texture for rendering.
         */
        explicit EffectSystem(SDL_Renderer *renderer) : _renderer(renderer) {
            // Pre-allocate particle vectors to avoid reallocations
            _activeParticles.reserve(1000);
            _particlePool.reserve(1000);

            // Create a simple particle texture for better performance
            createParticleTexture();
        }

        /**
         * @brief Destructor that cleans up resources used by the EffectSystem.
         * This destructor frees the particle texture and cleans up the particle pool
         * to prevent memory leaks.
         */
        ~EffectSystem() {
            if (_particleTexture) {
            SDL_DestroyTexture(_particleTexture);
            }

            // Clean up particle pool
            for (auto *particle : _particlePool) {
            delete particle;
            }
        }

        // --- Public Methods ---

        /**
         * @brief Adds a burst effect at the specified tile coordinates.
         * @param tileX The X coordinate of the tile (in board units).
         * @param tileY The Y coordinate of the tile (in board units).
         * @param color The color of the burst particles.
         * @param particleCount Number of particles in the burst (default is 12).
         * @param duration Duration of the burst effect in seconds (default is 0.6f).
         * @param topLayer Whether to render this burst on the top layer (default is
         * false).
         */
        void addTileBurst(int tileX, int tileY, const SDL_Color &color,
                            int particleCount = 12, float duration = 0.6f,
                            bool topLayer = false) {
            Burst b;
            b.tileX = static_cast<float>(tileX);
            b.tileY = static_cast<float>(tileY);
            b.duration = duration;
            b.elapsed = 0.f;
            b.color = color;
            b.top = topLayer;

            std::mt19937 rng{std::random_device{}()};
            std::uniform_real_distribution<float> angleDist(0.f, 2.f * M_PI);
            std::uniform_real_distribution<float> speedDist(40.f, 120.f);
            std::uniform_real_distribution<float> sizeDist(3.f, 6.f);
            std::uniform_real_distribution<float> lifeDist(0.4f, duration);

            for (int i = 0; i < particleCount; ++i) {
            float angle = angleDist(rng);
            float speed = speedDist(rng);

            // Get particle from pool or create new one
            Particle *p = getParticleFromPool();
            if (!p)
                continue;

            p->dx = std::cos(angle) * speed;
            p->dy = std::sin(angle) * speed;
            p->size = sizeDist(rng);
            p->life = lifeDist(rng);
            p->elapsed = 0.f;
            p->color = color;
            p->active = true;

            b.particles.push_back(p);
            }
            _bursts.push_back(std::move(b));
        }

        /**
         * @brief Adds a ring effect at the specified tile coordinates.
         * @param tileX The X coordinate of the tile (in board units).
         * @param tileY The Y coordinate of the tile (in board units).
         * @param color The color of the ring.
         * @param radiusMultiplier Multiplier for the ring radius relative to cell
         * size (default is 1.2f).
         * @param duration Duration of the ring effect in seconds (default is 0.8f).
         * @param topLayer Whether to render this ring on the top layer (default is
         * true).
         */
        void addTileRing(int tileX, int tileY, const SDL_Color &color,
                        float radiusMultiplier = 1.2f, float duration = 0.8f,
                        bool topLayer = true) {
            Ring r;
            r.tileX = static_cast<float>(tileX);
            r.tileY = static_cast<float>(tileY);
            r.duration = duration;
            r.elapsed = 0.f;
            r.color = color;
            r.top = topLayer;
            r.radiusMult = radiusMultiplier;
            _rings.push_back(std::move(r));
        }

        /**
         * @brief Updates the effect system, processing all active bursts and
         * particles.
         * @param dt Delta time in seconds since the last update.
         * This method updates the elapsed time for each burst and particle,
         * deactivating particles that have exceeded their lifetime. It also manages
         * the active particles list and updates ring effects.
         */
        void update(float dt) {
            // Update bursts
            for (auto it = _bursts.begin(); it != _bursts.end();) {
            it->elapsed += dt;
            if (it->elapsed >= it->duration) {
                // Return particles to pool
                for (auto *p : it->particles) {
                returnParticleToPool(p);
                }
                it = _bursts.erase(it);
            } else {
                for (auto *p : it->particles) {
                if (p->active) {
                    p->elapsed += dt;
                    if (p->elapsed >= p->life) {
                    p->active = false;
                    }
                }
                }
                ++it;
            }
            }

            // Update active particles list
            _activeParticles.clear();
            for (const auto &burst : _bursts) {
            for (auto *p : burst.particles) {
                if (p->active) {
                _activeParticles.push_back(p);
                }
            }
            }

            // Update rings
            updateRings(dt);
        }

        /**
         * @brief Renders all active effects to the screen.
         * @param viewportWidth Width of the viewport in pixels.
         * @param viewportHeight Height of the viewport in pixels.
         * @param boardW Width of the game board in tiles.
         * @param boardH Height of the game board in tiles.
         * @param topLayer Whether to render on the top layer (default is false).
         * This method calculates cell dimensions based on the viewport size and board
         * dimensions, then renders all active particles and rings.
         */
        void render(int viewportWidth, int viewportHeight, int boardW, int boardH,
                    bool topLayer = false) {
            if (boardW <= 0 || boardH <= 0)
            return;

            float cellW = static_cast<float>(viewportWidth) / boardW;
            float cellH = static_cast<float>(viewportHeight) / boardH;

            // Batch render all particles
            if (_particleTexture) {
            renderParticlesWithTexture(cellW, cellH, topLayer);
            } else {
            renderParticlesAsRectangles(cellW, cellH, topLayer);
            }

            // Render rings
            renderRings(cellW, cellH, topLayer);
    }

    private:
        // --- Nested Structs ---

        struct Particle {
            float dx, dy;  // velocity pixel/sec
            float size;
            float life;    // seconds
            float elapsed; // seconds
            SDL_Color color;
            bool active = false;
        };

        struct Burst {
            float tileX, tileY;
            float duration;
            float elapsed;
            SDL_Color color;
            bool top;
            std::vector<Particle *> particles;
        };

        struct Ring {
            float tileX, tileY;
            float duration;
            float elapsed;
            SDL_Color color;
            bool top;
            float radiusMult; // in cell size units
        };

        // --- Private Helper Methods ---

        /**
         * @brief Gets a particle from the pool or creates a new one if the pool is
         * empty.
         * @return A pointer to a Particle object.
         */
        Particle *getParticleFromPool();

        /**
         * @brief Returns a particle back to the pool for reuse.
         * @param p Pointer to the Particle to return.
         * This method resets the particle's state and adds it back to the pool.
         */
        void returnParticleToPool(Particle *p);

        /**
         * @brief Creates a simple particle texture for rendering.
         * This method initializes a texture with a solid color that can be used for
         * rendering particles efficiently.
         */
        void createParticleTexture();

        /**
         * @brief Renders all active particles using a texture.
         * @param cellW Width of a cell in pixels.
         * @param cellH Height of a cell in pixels.
         * @param topLayer Whether to render on the top layer.
         * This method batches rendering of particles using a pre-created texture for
         * better performance.
         */
        void renderParticlesWithTexture(float cellW, float cellH, bool topLayer);

        /**
         * @brief Renders all active particles as rectangles.
         * @param cellW Width of a cell in pixels.
         * @param cellH Height of a cell in pixels.
         * @param topLayer Whether to render on the top layer.
         * This method renders each particle as a rectangle, which is less efficient
         * than using a texture but can be useful for debugging or fallback rendering.
         */
        void renderParticlesAsRectangles(float cellW, float cellH, bool topLayer);

        /**
         * @brief Renders all active rings.
         * @param cellW Width of a cell in pixels.
         * @param cellH Height of a cell in pixels.
         * @param topLayer Whether to render on the top layer.
         * This method iterates through all active rings and renders them based on
         * their elapsed time and duration.
         */
        void renderRings(float cellW, float cellH, bool topLayer);

        /**
         * @brief Updates the state of all active rings.
         * @param dt Delta time in seconds since the last update.
         * This method processes each ring, updating its elapsed time and removing it
         * if it has completed its duration.
         */
        void updateRings(float dt);

        // --- Private Members ---

        SDL_Renderer *_renderer;
        std::vector<Burst> _bursts;

        std::vector<Particle *> _activeParticles;
        std::vector<Particle *> _particlePool;
        SDL_Texture *_particleTexture = nullptr;

        std::vector<Ring> _rings;
};
