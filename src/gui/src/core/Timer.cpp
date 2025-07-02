/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin [WSL: Ubuntu]
** File description:
** Timer
*/

#include "Timer.hpp"

Timer::Timer() {
    reset();
}

void Timer::reset() {
    startTime = std::chrono::high_resolution_clock::now();
}

float Timer::getElapsedSeconds() const {
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<float>(now - startTime).count();
}

float Timer::getElapsedMilliseconds() const {
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<float, std::milli>(now - startTime).count();
}
