#include "Texture.h"
#include <SDL3_image/SDL_image.h>

Texture::Texture(SDL_Renderer* renderer, const char* path)
    : texture_(IMG_LoadTexture(renderer, path))
{
    if (texture_ == nullptr)
        SDL_Log("Couldn't load %s: %s", path, SDL_GetError());
}

Texture::~Texture()
{
    if (texture_ != nullptr)
        SDL_DestroyTexture(texture_);
}

bool Texture::isLoaded() const
{
    return texture_ != nullptr;
}

void Texture::draw(SDL_Renderer* renderer, const SDL_FRect* src,
                   const SDL_FRect* dst, SDL_FlipMode flip) const
{
    SDL_RenderTextureRotated(renderer, texture_, src, dst, 0.0, nullptr, flip);
}
