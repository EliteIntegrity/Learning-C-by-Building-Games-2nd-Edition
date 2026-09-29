#pragma once
#include <SDL3/SDL.h>
#include <string>   // std::string, for the file's path

// One sound effect, loaded from a WAV file when it's made, and ready to
// play at any moment
class Sound
{
public:
    Sound(const std::string& path);
    ~Sound();

    Sound(const Sound&) = delete;
    Sound& operator=(const Sound&) = delete;

    bool isLoaded() const;
    void play() const;

private:
    Uint8* samples_ = nullptr;            // the sound itself
    Uint32 length_ = 0;                   // its size, in bytes
    SDL_AudioStream* stream_ = nullptr;   // the way to the speakers
};

// Every sound in the game, each loaded from its own file
struct Sounds
{
    Sound hit{ "assets/hit.wav" };         // the player hits a monster
    Sound kill{ "assets/kill.wav" };       // and kills it
    Sound hurt{ "assets/hurt.wav" };       // a monster hits the player
    Sound pickup{ "assets/pickup.wav" };   // anything picked up
    Sound drink{ "assets/drink.wav" };     // a potion
    Sound stairs{ "assets/stairs.wav" };   // going down
    Sound scroll{ "assets/scroll.wav" };   // reading a scroll
};
