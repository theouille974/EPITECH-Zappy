/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin [WSL: Ubuntu]
** File description:
** EventDispatcher
*/

#pragma once
#include <unordered_map>
#include <functional>
#include <vector>
#include <string>

class EventDispatcher {
    public:
        /**
         * EventDispatcher constructor.
         * Initializes the event dispatcher.
         */
        using Callback = std::function<void()>;

        /**
         * Subscribes a callback to an event.
         * @param eventName Name of the event to subscribe to.
         * @param callback Function to call when the event is dispatched.
         */
        void subscribe(const std::string& eventName, Callback callback);
        /**
         * Unsubscribes a callback from an event.
         * @param eventName Name of the event to unsubscribe from.
         * @param callback Function to remove from the event's listeners.
         */
        void dispatch(const std::string& eventName);

    private:
        std::unordered_map<std::string, std::vector<Callback>> listeners;
};
