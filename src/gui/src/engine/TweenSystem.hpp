/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** TweenSystem
*/

#pragma once
#include <functional>
#include <vector>
#include <algorithm>

class TweenSystem {
    public:
        // Easing Functions
        /**
         * @brief Default easing function that performs linear interpolation.
         * @param t The ratio of elapsed time to total duration (0.0 to 1.0).
         * @return The interpolated value at time t.
         */
        static float linear(float t) { return t; }

        // Public Methods
        /**
         * @brief Adds a new tween to animate a float value over time.
         * @param value Pointer to the float value to animate.
         * @param target The target value to reach at the end of the animation.
         * @param duration The duration of the animation in seconds.
         * @param easing Optional easing function to use for the animation (default is linear).
         * @note If the value is null or duration is less than or equal to zero, the value
         * @note is set directly to the target without animation.
         */
        void add(float* value, float target, float duration,
                std::function<float(float)> easing = linear) {
            if (!value || duration <= 0.0f) {
                if (value) *value = target;
                return;
            }
            Tween t{value, *value, target, duration, 0.f, easing};
            _tweens.push_back(t);
        }

        /**
         * @brief Updates all active tweens based on the elapsed time.
         * @param dt The delta time in seconds since the last update.
         * @note This method processes each tween, updating its value based on the elapsed
         * time and removing it if the animation is complete.
         */
        void update(float dt) {
            for (auto it = _tweens.begin(); it != _tweens.end();) {
                Tween& tw = *it;
                tw.elapsed += dt;
                float ratio = tw.elapsed / tw.duration;
                if (ratio > 1.f) ratio = 1.f;
                float k = tw.easing ? tw.easing(ratio) : ratio;
                *(tw.value) = tw.start + (tw.target - tw.start) * k;
                if (tw.elapsed >= tw.duration) {
                    it = _tweens.erase(it);
                } else {
                    ++it;
                }
            }
        }

        /**
         * @brief Clears all active tweens.
         * @note This method removes all tweens from the system, effectively resetting it.
         */
        void clear() { _tweens.clear(); }

        /**
         * @brief Removes a specific target from the tween system.
         * @param value Pointer to the float value to remove from tweens.
         * @note This method searches for tweens that target the specified value and removes them.
         */
        void removeTarget(float* value) {
            _tweens.erase(std::remove_if(_tweens.begin(), _tweens.end(),
                [value](const Tween& t){ return t.value == value; }), _tweens.end());
        }

        /**
         * @brief Checks if there are any active tweens.
         * @return True if there are no active tweens, false otherwise.
         */
        bool empty() const { return _tweens.empty(); }

    private:
        // Private Structs
        struct Tween {
            float* value;
            float  start;
            float  target;
            float  duration;
            float  elapsed;
            std::function<float(float)> easing;
        };

        // Private Member Variables
        std::vector<Tween> _tweens;
};
