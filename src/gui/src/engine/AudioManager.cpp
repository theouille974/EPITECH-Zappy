/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin [WSL: Ubuntu]
** File description:
** AudioManager
*/

#include "AudioManager.hpp"
#include <iostream>

AudioManager::AudioManager() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer Init Error: " << Mix_GetError() << std::endl;
    }
}

AudioManager::~AudioManager() {
    for (auto& [_, chunk] : sfxMap)
        Mix_FreeChunk(chunk);
    for (auto& [_, music] : musicMap)
        Mix_FreeMusic(music);

    Mix_CloseAudio();
}

bool AudioManager::loadSFX(const std::string& id, const std::string& path) {
    Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
    if (!chunk) {
        std::cerr << "Failed to load SFX " << path << ": " << Mix_GetError() << std::endl;
        return false;
    }
    sfxMap[id] = chunk;
    return true;
}

bool AudioManager::loadMusic(const std::string& id, const std::string& path) {
    Mix_Music* music = Mix_LoadMUS(path.c_str());
    if (!music) {
        std::cerr << "Failed to load music " << path << ": " << Mix_GetError() << std::endl;
        return false;
    }
    musicMap[id] = music;
    return true;
}

void AudioManager::playSFX(const std::string& id, int loops) {
    auto it = sfxMap.find(id);
    if (it != sfxMap.end())
        Mix_PlayChannel(-1, it->second, loops);
}

void AudioManager::playMusic(const std::string& id, int loops) {
    auto it = musicMap.find(id);
    if (it != musicMap.end())
        Mix_PlayMusic(it->second, loops);
}

void AudioManager::stopMusic() {
    Mix_HaltMusic();
}

void AudioManager::setSFXVolume(int volume) {
    Mix_Volume(-1, volume);
}

void AudioManager::setMusicVolume(int volume) {
    Mix_VolumeMusic(volume);
}
