/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin [WSL: Ubuntu]
** File description:
** Timer
*/

#pragma once
#include <chrono>

class Timer {
    public:
        /**
         * @brief Constructs a Timer object and starts the timer.
         */
        Timer();

        /**
         * @brief Resets the timer to start counting from zero again.
         */
        void reset();
        /**
         * @brief Gets the elapsed time in seconds since the timer was started or last reset.
         * @return The elapsed time in seconds.
         */
        float getElapsedSeconds() const;
        /**
         * @brief Gets the elapsed time in milliseconds since the timer was started or last reset.
         * @return The elapsed time in milliseconds.
         */
        float getElapsedMilliseconds() const;

    private:
        std::chrono::high_resolution_clock::time_point startTime;
};
