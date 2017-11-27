#include <iostream>
#include "MusicLoader.h"

MusicLoader::MusicLoader() {
    Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096);

    music = Mix_LoadMUS(MAIN_MUSIC_PATH);
    for (int i = 0; i < TOTAL_CHUNKS; ++i) {
        effects[i] = Mix_LoadWAV(std::string(
                "/etc/TowerDefense/Resources/" + std::to_string(i) + ".ogg").c_str());
    }
}

void MusicLoader::playMusic() {
    Mix_PlayMusic(music, -1);
}

Mix_Chunk* MusicLoader::getChunk(int chunkName) {
    return effects[chunkName];
}

MusicLoader::~MusicLoader() {
    Mix_FreeMusic(music);
    for (auto& chunk : effects) {
        Mix_FreeChunk(chunk.second);
    }
    Mix_CloseAudio();
}
