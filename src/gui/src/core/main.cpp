/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin [WSL: Ubuntu]
** File description:
** main with EventDispatcher and Timer
*/
#include "App.hpp"
#include "EventDispatcher.hpp"
#include "Timer.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <regex>

bool isValidIPv4(const std::string& ip);

struct GuiConfig {
    int port = 4242;
    std::string hostname = "127.0.0.1";
};

void printUsage(const char* programName) {
    std::cout << "USAGE: " << programName << " -p port -h machine" << std::endl;
    std::cout << "option description" << std::endl;
    std::cout << "-p port        port number" << std::endl;
    std::cout << "-h machine     hostname of the server" << std::endl;
}

bool parseArguments(int argc, char* argv[], GuiConfig& config) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "help") == 0) {
            printUsage(argv[0]);
            return false;
        }
        else if (strcmp(argv[i], "-p") == 0) {
            if (i + 1 >= argc) {
                std::cerr << "Error: -p requires a port number" << std::endl;
                return false;
            }
            try {
                config.port = std::stoi(argv[i + 1]);
                if (config.port <= 0 || config.port > 65535) {
                    std::cerr << "Error: Port must be between 1 and 65535" << std::endl;
                    return false;
                }
            } catch (const std::exception&) {
                std::cerr << "Error: Invalid port number: " << argv[i + 1] << std::endl;
                return false;
            }
            i++;
        }
        else if (strcmp(argv[i], "-h") == 0) {
            if (i + 1 >= argc) {
                std::cerr << "Error: -h requires a hostname" << std::endl;
                return false;
            }
            std::string hostname = argv[i + 1];
            if (isValidIPv4(hostname)) {
                config.hostname = hostname;
            } else {
                std::cerr << "Warning: Invalid IPv4 address '" << hostname << "', using 127.0.0.1" << std::endl;
                config.hostname = "127.0.0.1";
            }
            i++;
        }
        else {
            std::cerr << "Error: Unknown argument: " << argv[i] << std::endl;
            printUsage(argv[0]);
            return false;
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    GuiConfig config;
    
    if (!parseArguments(argc, argv, config)) {
        return 1;
    }
    
    EventDispatcher dispatcher;
    Timer timer;

    dispatcher.subscribe("hello", []() {
        // Event handler
    });

    timer.reset();
    dispatcher.dispatch("hello");
    
    App app(config.hostname, config.port);
    app.run();

    return 0;
}