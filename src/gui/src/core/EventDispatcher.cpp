/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin [WSL: Ubuntu]
** File description:
** EventDispatcher
*/

#include "EventDispatcher.hpp"

void EventDispatcher::subscribe(const std::string& eventName, Callback callback) {
    listeners[eventName].emplace_back(std::move(callback));
}

void EventDispatcher::dispatch(const std::string& eventName) {
    auto it = listeners.find(eventName);
    if (it != listeners.end()) {
        for (auto& callback : it->second) {
            callback();
        }
    }
}
