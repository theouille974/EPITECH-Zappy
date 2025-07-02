/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** ServerUpdateManager
*/

#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

class ServerUpdateManager {
    public:
        // --- Nested Structs and Enums ---

        /**
         * Represents resources available on the board or in a player's inventory.
         * Each tile has a set of resources represented by an array of integers.
         */
        struct Resources {
            int qty[7] = {0};
        };

        /**
         * Represents a player in the game.
         * Each player has an ID, position, orientation, level, team affiliation,
         * inventory, and life status.
         */
        struct Player {
            int id;
            int x{0};
            int y{0};
            int orientation{1};
            int level{1};
            std::string team;
            Resources inv;
            bool alive{true};
            int lifeMeter{0}; // Life meter in seconds (time to live)
        };

        /**
         * Represents an egg in the game.
         * Eggs are spawned by players and hatch into new players after a certain time.
         */
        struct Egg {
            int id;           // #e dans le protocole
            int playerId;     // #n du joueur qui l'a pondu
            int x{0};
            int y{0};
            std::string team; // Équipe héritée du joueur parent
            bool alive{true}; // false quand edi reçu
        };

        /**
         * Simple broadcast struct for storing last messages.
         */
        struct Broadcast {
            int fromId;
            std::string message;
        };

        /**
         * Enumerates the types of events that can occur in the game.
         */
        enum class EventType {
            PLAYER_LOGIN,
            PLAYER_LOGOUT,
            INCANTATION_START,
            RESOURCE_DROP,
            PLAYER_LEVEL_UP,
            BROADCAST
        };

        /**
         * Represents an event that occurred in the game.
         * Events can be player logins, logouts, incantations, resource drops, or
         * level-ups.
         */
        struct Event {
            EventType type;
            int id;
            int x;
            int y;
        };

        // --- Constructor and Destructor ---

        /**
         * Constructor that initializes the server update manager with a host and port.
         * @param host The hostname or IP address of the server.
         * @param port The port number to connect to.
         */
        ServerUpdateManager(const std::string &host, uint16_t port);

        /**
         * Destructor that cleans up resources.
         */
        ~ServerUpdateManager();

        // --- Connection Management ---

        /**
         * Connects to the server.
         * @return True if the connection was successful, false otherwise.
         */
        bool connect();

        /**
         * Disconnects from the server and clears all data.
         */
        void disconnect();

        /**
         * Polls the server for updates and processes incoming data.
         * This should be called regularly to keep the state updated.
         */
        void poll();

        // --- Board Information Getters ---

        /**
         * Returns the width of the game board.
         * @return The width of the board.
         */
        int getBoardWidth() const { return _boardWidth; }

        /**
         * Returns the height of the game board.
         * @return The height of the board.
         */
        int getBoardHeight() const { return _boardHeight; }

        /**
         * Returns the names of the teams participating in the game.
         * @return A vector of team names.
         */
        const std::vector<std::string> &getTeamNames() const { return _teamNames; }

        /**
         * Returns the resources at a specific tile on the board.
         * @param x The x coordinate of the tile.
         * @param y The y coordinate of the tile.
         * @return A pointer to the resources at the specified tile, or nullptr if out
         * of bounds.
         */
        const Resources *getTileResources(int x, int y) const {
            if (x < 0 || y < 0 || x >= _boardWidth || y >= _boardHeight)
            return nullptr;
            return &_resources[y * _boardWidth + x];
        }

        /**
         * Returns the total resources available across all tiles.
         * This aggregates the resources from all tiles on the board.
         * @return A Resources object containing the total quantities of each resource
         * type.
         */
        Resources getTotalResources() const;

        // --- Player Information Getters ---

        /**
         * Returns a map of all players currently in the game.
         * @return A map where keys are player IDs and values are Player objects.
         */
        const std::unordered_map<int, Player> &getPlayers() const { return _players; }

        /**
         * Returns a pointer to the Player object with the specified ID.
         * @param id The ID of the player to retrieve.
         * @return A pointer to the Player object, or nullptr if not found.
         */
        const Player *getPlayer(int id) const {
            auto it = _players.find(id);
            return it == _players.end() ? nullptr : &it->second;
        }

        // --- Egg Information Getters ---

        /**
         * Returns a map of all eggs currently in the game.
         * @return A map where keys are egg IDs and values are Egg objects.
         */
        const std::unordered_map<int, Egg> &getEggs() const { return _eggs; }

        /**
         * Returns a pointer to the Egg object with the specified ID.
         * @param id The ID of the egg to retrieve.
         * @return A pointer to the Egg object, or nullptr if not found.
         */
        const Egg *getEgg(int id) const {
            auto it = _eggs.find(id);
            return it == _eggs.end() ? nullptr : &it->second;
        }

        /**
         * Returns a vector of all eggs currently on the board at the specified
         * coordinates.
         * @param x The x coordinate of the tile.
         * @param y The y coordinate of the tile.
         * @return A vector of pointers to Egg objects at the specified tile.
         */
        std::vector<const Egg *> getEggsAt(int x, int y) const {
            std::vector<const Egg *> result;
            for (const auto &[id, egg] : _eggs) {
            if (egg.alive && egg.x == x && egg.y == y) {
                result.push_back(&egg);
            }
            }
            return result;
        }

        // --- Game State Getters ---

        /**
         * Returns the time unit of the game.
         * This is used to synchronize game state updates.
         * @return The current time unit.
         */
        int getTimeUnit() const { return _timeUnit; }

        /**
         * Returns the broadcasts sent by players.
         * Broadcasts are messages sent by players that can be displayed in the game.
         * @return A vector of Broadcast objects containing the messages.
         */
        const std::vector<Broadcast> &getBroadcasts() const { return _broadcasts; }

        /**
         * Returns a vector of events that have occurred since the last poll.
         * This allows the game to react to player actions and other events.
         * @return A vector of Event objects containing the events that occurred.
         */
        std::vector<Event> consumeEvents() {
            auto tmp = _events;
            _events.clear();
            return tmp;
        }

    private:
        // --- Private Methods ---

        /**
         * Processes a line of input from the server.
         * This method parses the line and updates the internal state accordingly.
         * @param line The line of input to process.
         */
        void processLine(const std::string &line);

        // --- Private Members ---

        std::string _host;
        uint16_t _port;
        int _sock{-1};
        std::string _readBuffer;

        int _boardWidth{0};
        int _boardHeight{0};
        std::vector<std::string> _teamNames;
        std::vector<Resources> _resources;
        std::unordered_map<int, Player> _players;

        std::unordered_map<int, Egg> _eggs;
        int _timeUnit{0};
        std::vector<Broadcast> _broadcasts;
        std::vector<Event> _events;
};
