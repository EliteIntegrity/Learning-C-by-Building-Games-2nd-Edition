#pragma once
#include <SDL3/SDL.h>

// A picture, loaded from a file when a Texture is made, and destroyed by
// its destructor, so it can't be forgotten
class Texture
{
public:
    Texture(SDL_Renderer* renderer, const char* path);
    ~Texture();

    // One picture, one owner, so a Texture can't be copied
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    bool isLoaded() const;

    // Draw the part of the picture in src into dst, mirrored if flip says
    // so. A nullptr for src means the whole picture, and for dst, the
    // whole window
    void draw(SDL_Renderer* renderer, const SDL_FRect* src,
              const SDL_FRect* dst, SDL_FlipMode flip = SDL_FLIP_NONE) const;

private:
    SDL_Texture* texture_;
};
