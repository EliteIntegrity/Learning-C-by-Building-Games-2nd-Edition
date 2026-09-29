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
                   const SDL_FRect* dst, SDL_FlipMode flip,
                   SDL_Color tint) const
{
    // Every drawing sets its own tint, so none is left over from the last
    SDL_SetTextureColorMod(texture_, tint.r, tint.g, tint.b);
    SDL_SetTextureAlphaMod(texture_, tint.a);
    SDL_RenderTextureRotated(renderer, texture_, src, dst, 0.0, nullptr, flip);
}
