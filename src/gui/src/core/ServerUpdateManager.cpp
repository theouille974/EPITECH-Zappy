/*
** EPITECH PROJECT, 2025
** B-YEP-400
** File description:
** ServerUpdateManager
*/

#include "ServerUpdateManager.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h> 
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <unordered_map>
#include <sys/select.h>

ServerUpdateManager::ServerUpdateManager(const std::string& host, uint16_t port)
    : _host(host), _port(port) {}

ServerUpdateManager::~ServerUpdateManager() {
    if (_sock != -1)
        close(_sock);
}

bool ServerUpdateManager::connect() {
    if (_sock != -1) {
        disconnect();
    }

    _sock = socket(AF_INET, SOCK_STREAM, 0);
    if (_sock == -1) {
        perror("socket");
        return false;
    }

    int flags = fcntl(_sock, F_GETFL, 0);
    if (flags != -1) {
        fcntl(_sock, F_SETFL, flags | O_NONBLOCK);
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);

    if (inet_pton(AF_INET, _host.c_str(), &addr.sin_addr) != 1) {
        struct hostent* host_entry = gethostbyname(_host.c_str());
        if (host_entry == nullptr) {
            std::cerr << "Cannot resolve hostname: " << _host << std::endl;
            close(_sock);
            _sock = -1;
            return false;
        }
        memcpy(&addr.sin_addr, host_entry->h_addr_list[0], host_entry->h_length);
    }

    int res = ::connect(_sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    if (res == -1 && errno == EINPROGRESS) {
        fd_set wfds;
        FD_ZERO(&wfds);
        FD_SET(_sock, &wfds);
        struct timeval tv {2, 0};
        res = select(_sock + 1, nullptr, &wfds, nullptr, &tv);
        if (res <= 0) {
            std::cerr << "Connection timed out to " << _host << ":" << _port << std::endl;
            close(_sock);
            _sock = -1;
            return false;
        }
        int so_error = 0;
        socklen_t len = sizeof(so_error);
        if (getsockopt(_sock, SOL_SOCKET, SO_ERROR, &so_error, &len) == -1 || so_error != 0) {
            std::cerr << "Failed to connect: " << strerror(so_error) << std::endl;
            close(_sock);
            _sock = -1;
            return false;
        }
    } else if (res == -1) {
        perror("connect");
        close(_sock);
        _sock = -1;
        return false;
    }

    const char* hello = "GRAPHIC\n";
    ::send(_sock, hello, strlen(hello), 0);

    return true;
}

void ServerUpdateManager::disconnect() {
    if (_sock != -1) {
        close(_sock);
        _sock = -1;
    }
    
    _readBuffer.clear();
    _boardWidth = 0;
    _boardHeight = 0;
    _teamNames.clear();
    _resources.clear();
    _players.clear();
    _eggs.clear();
    _timeUnit = 0;
    _broadcasts.clear();
}

void ServerUpdateManager::poll() {
    if (_sock == -1)
        return;

    char buf[1024];
    ssize_t n = recv(_sock, buf, sizeof(buf), 0);
    while (n > 0) {
        _readBuffer.append(buf, n);
        size_t pos;
        while ((pos = _readBuffer.find('\n')) != std::string::npos) {
            std::string line = _readBuffer.substr(0, pos);
            if (!line.empty() && line.back() == '\r')
                line.pop_back();
            processLine(line);
            _readBuffer.erase(0, pos + 1);
        }
        n = recv(_sock, buf, sizeof(buf), 0);
    }
}

void ServerUpdateManager::processLine(const std::string& line) {
    if (line.rfind("msz", 0) == 0) {
        int w, h;
        if (sscanf(line.c_str(), "msz %d %d", &w, &h) == 2) {
            _boardWidth = w;
            _boardHeight = h;
            _resources.assign(_boardWidth * _boardHeight, {});
        }
    } else if (line.rfind("tna", 0) == 0) {
        size_t spacePos = line.find(' ');
        if (spacePos != std::string::npos && spacePos + 1 < line.size()) {
            std::string name = line.substr(spacePos + 1);
            _teamNames.emplace_back(name);
        }
    } else if (line.rfind("bct", 0) == 0) {
        int x, y, q0,q1,q2,q3,q4,q5,q6;
        if (sscanf(line.c_str(), "bct %d %d %d %d %d %d %d %d %d", &x,&y,&q0,&q1,&q2,&q3,&q4,&q5,&q6) == 9) {
            if (x >=0 && y>=0 && x < _boardWidth && y < _boardHeight) {
                Resources& r = _resources[y * _boardWidth + x];
                r.qty[0]=q0; r.qty[1]=q1; r.qty[2]=q2; r.qty[3]=q3; r.qty[4]=q4; r.qty[5]=q5; r.qty[6]=q6;
            }
        }
    } else if (line.rfind("pnw", 0) == 0) {
        int id,x,y,o,l; char team[256];
        if (sscanf(line.c_str(), "pnw #%d %d %d %d %d %255s", &id,&x,&y,&o,&l,team) == 6) {
            Player& p = _players[id];
            p.id=id; p.x=x; p.y=y; p.orientation=o; p.level=l; p.team=team; p.alive=true;
            _events.push_back({ServerUpdateManager::EventType::PLAYER_LOGIN,id,x,y});
        }
    } else if (line.rfind("ppo",0)==0) {
        int id,x,y,o;
        if (sscanf(line.c_str(), "ppo #%d %d %d %d", &id,&x,&y,&o)==4) {
            auto it=_players.find(id);
            if(it!=_players.end()) { it->second.x=x; it->second.y=y; it->second.orientation=o; }
        }
    } else if (line.rfind("plv",0)==0) {
        int id,l;
        if (sscanf(line.c_str(), "plv #%d %d", &id,&l)==2) {
            auto it=_players.find(id);
            if(it!=_players.end()) {
                int oldLevel = it->second.level;
                it->second.level=l;
                if (l > oldLevel) {
                    _events.push_back({ServerUpdateManager::EventType::PLAYER_LEVEL_UP, id, it->second.x, it->second.y});
                }
            }
        }
    } else if (line.rfind("pin",0)==0) {
        int id,x,y,q0,q1,q2,q3,q4,q5,q6;
        if (sscanf(line.c_str(), "pin #%d %d %d %d %d %d %d %d %d %d", &id,&x,&y,&q0,&q1,&q2,&q3,&q4,&q5,&q6)==10) {
            auto& p=_players[id];
            p.id=id; p.x=x; p.y=y;
            p.inv.qty[0]=q0; p.inv.qty[1]=q1; p.inv.qty[2]=q2; p.inv.qty[3]=q3; p.inv.qty[4]=q4; p.inv.qty[5]=q5; p.inv.qty[6]=q6;
        }
    } else if (line.rfind("pdi",0)==0) {
        int id; if (sscanf(line.c_str(), "pdi #%d", &id)==1) { 
            auto itP=_players.find(id);
            int px=0,py=0; if(itP!=_players.end()){ px=itP->second.x; py=itP->second.y; }
            _events.push_back({ServerUpdateManager::EventType::PLAYER_LOGOUT,id,px,py});
            _players.erase(id);} 
    } else if (line.rfind("enw", 0) == 0) {
        int eggId, playerId, x, y;
        if (sscanf(line.c_str(), "enw #%d #%d %d %d", &eggId, &playerId, &x, &y) == 4) {
            Egg& egg = _eggs[eggId];
            egg.id = eggId;
            egg.playerId = playerId;
            egg.x = x;
            egg.y = y;
            egg.alive = true;
            
            auto playerIt = _players.find(playerId);
            if (playerIt != _players.end()) {
                egg.team = playerIt->second.team;
            }
        }
        
    } else if (line.rfind("ebo", 0) == 0) {
        int eggId;
        if (sscanf(line.c_str(), "ebo #%d", &eggId) == 1) {
            auto eggIt = _eggs.find(eggId);
            if (eggIt != _eggs.end()) {
            }
        }
        
    } else if (line.rfind("edi", 0) == 0) {
        int eggId;
        if (sscanf(line.c_str(), "edi #%d", &eggId) == 1) {
            auto eggIt = _eggs.find(eggId);
            if (eggIt != _eggs.end()) {
                _eggs.erase(eggId);
            }
        }
    } else if (line.rfind("pic",0)==0) {
        int x,y,l; // ignore ids list
        if(sscanf(line.c_str(), "pic %d %d %d", &x,&y,&l)==3){
            _events.push_back({ServerUpdateManager::EventType::INCANTATION_START, -1, x, y});
        }
    } else if (line.rfind("pdr",0)==0) {
        int id,i;
        if(sscanf(line.c_str(), "pdr #%d %d", &id,&i)==2){
            auto it=_players.find(id); int px=0,py=0; if(it!=_players.end()){px=it->second.x;py=it->second.y;}
            _events.push_back({ServerUpdateManager::EventType::RESOURCE_DROP,id,px,py});
        }
    } else if (line.rfind("pbc", 0) == 0) {
        size_t idStart = line.find('#');
        if (idStart != std::string::npos) {
            size_t spacePos = line.find(' ', idStart);
            int id = 0;
            if (spacePos != std::string::npos) {
                try {
                    id = std::stoi(line.substr(idStart + 1, spacePos - idStart - 1));
                } catch (const std::exception &) { id = 0; }
                std::string msg = line.substr(spacePos + 1);
                _broadcasts.push_back({id, msg});
                auto it = _players.find(id);
                int px = 0, py = 0;
                if (it != _players.end()) {
                    px = it->second.x;
                    py = it->second.y;
                }
                _events.push_back({ServerUpdateManager::EventType::BROADCAST, id, px, py});
            }
        }
    }
}

ServerUpdateManager::Resources ServerUpdateManager::getTotalResources() const {
    Resources total{};
    for (const auto& r : _resources) {
        for (int i = 0; i < 7; ++i)
            total.qty[i] += r.qty[i];
    }
    return total;
}