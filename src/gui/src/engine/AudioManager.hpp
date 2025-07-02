/*
** EPITECH PROJECT, 2025
** B-YEP-400-MPL-4-1-zappy-louis.filhol-valantin
** File description:
** AudioManager
*/

#pragma once
#include <SDL2/SDL_mixer.h>
#include <string>
#include <unordered_map>

class AudioManager {
    public:
        /**
         * @brief Constructs an AudioManager object and initializes SDL_mixer.
         * This constructor sets up the audio subsystem and prepares it for use.
         */
        AudioManager();

        /**
         * @brief Destructor that cleans up loaded audio resources.
         * This destructor frees all loaded sound effects and music to prevent memory leaks.
         */
        ~AudioManager();

        /**
         * @brief Loads a sound effect from the specified file path.
         * @param id Unique identifier for the sound effect.
         * @param path File path to the sound effect file.
         * @return True if the sound effect was loaded successfully, false otherwise.
         */
        bool loadSFX(const std::string& id, const std::string& path);
        /**
         * @brief Loads a music track from the specified file path.
         * @param id Unique identifier for the music track.
         * @param path File path to the music file.
         * @return True if the music was loaded successfully, false otherwise.
         */
        bool loadMusic(const std::string& id, const std::string& path);

        /**
         * @brief Plays a sound effect by its identifier.
         * @param id Unique identifier for the sound effect.
         * @param loops Number of times to loop the sound effect (0 for no looping).
         */
        void playSFX(const std::string& id, int loops = 0);
        /**
         * @brief Sets the volume for sound effects.
         * @param volume Volume level (0-128).
         */
        void setSFXVolume(int volume);

        /**
         * @brief Stops all currently playing sound effects.
         */
        void playMusic(const std::string& id, int loops = -1);
        /**
         * @brief Stops the currently playing music.
         * This method stops any music that is currently playing.
         */
        void stopMusic();

        /**
         * @brief Sets the volume for music.
         * @param volume Volume level (0-128).
         */
        void setMusicVolume(int volume);

    private:
        std::unordered_map<std::string, Mix_Chunk*> sfxMap;
        std::unordered_map<std::string, Mix_Music*> musicMap;
};
