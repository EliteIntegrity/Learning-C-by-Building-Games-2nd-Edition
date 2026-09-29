#include "Sound.h"

// Loads the sound, and opens a stream to the speakers that can play it
Sound::Sound(const std::string& path)
{
    SDL_AudioSpec spec;
    if (!SDL_LoadWAV(path.c_str(), &spec, &samples_, &length_))
    {
        SDL_Log("Couldn't load %s: %s", path.c_str(), SDL_GetError());
        return;
    }

    stream_ = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
                                        &spec, nullptr, nullptr);
    if (!stream_)
    {
        SDL_Log("Couldn't open audio for %s: %s", path.c_str(),
                SDL_GetError());
        return;
    }

    // A new stream starts paused, so that nothing plays until it's ready
    SDL_ResumeAudioStreamDevice(stream_);
}

// Closes the stream, which stops the sound, and frees its memory
Sound::~Sound()
{
    if (stream_)
        SDL_DestroyAudioStream(stream_);
    SDL_free(samples_);
}

bool Sound::isLoaded() const
{
    return stream_ != nullptr;
}

// Plays the sound from the start, cutting off any of it still playing
void Sound::play() const
{
    if (!stream_)
        return;

    SDL_ClearAudioStream(stream_);
    SDL_PutAudioStreamData(stream_, samples_, static_cast<int>(length_));
}
