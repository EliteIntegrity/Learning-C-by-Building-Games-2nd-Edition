#include "GlyphCache.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <cmath>   // std::floor

GlyphCache::GlyphCache(SDL_Renderer* renderer, const std::string& fontPath,
                       float size)
{
    TTF_Font* font = TTF_OpenFont(fontPath.c_str(), size);
    if (!font)
    {
        SDL_Log("Couldn't open %s: %s", fontPath.c_str(), SDL_GetError());
        return;
    }

    // Draw each character once, in white, and keep it as a texture
    const SDL_Color white = { 255, 255, 255, 255 };
    for (char c = FIRST_GLYPH; c <= LAST_GLYPH; ++c)
    {
        SDL_Surface* surface = TTF_RenderGlyph_Blended(font, c, white);
        glyphs_[c - FIRST_GLYPH] =
            SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);
    }
    TTF_CloseFont(font);

    // In a monospaced font, every character is the same size, so the
    // first one gives us the size of them all
    SDL_GetTextureSize(glyphs_[0], &glyphW_, &glyphH_);
    loaded_ = true;
}

GlyphCache::~GlyphCache()
{
    for (SDL_Texture* glyph : glyphs_)
    {
        if (glyph)
            SDL_DestroyTexture(glyph);
    }
}

bool GlyphCache::isLoaded() const
{
    return loaded_;
}

// Draws one character in the middle of a cell
void GlyphCache::draw(SDL_Renderer* renderer, char c, Point cell,
                      SDL_Color color) const
{
    // Whole pixels only, so that the glyph stays sharp
    float x = cell.x * CELL_PX + std::floor((CELL_PX - glyphW_) / 2);
    float y = cell.y * CELL_PX + std::floor((CELL_PX - glyphH_) / 2);
    drawAt(renderer, c, x, y, color);
}

// Draws one character with its top-left corner at a pixel, tinted to a
// color. Characters without a glyph are skipped
void GlyphCache::drawAt(SDL_Renderer* renderer, char c, float x, float y,
                        SDL_Color color) const
{
    if (c < FIRST_GLYPH || c > LAST_GLYPH)
        return;

    SDL_Texture* glyph = glyphs_[c - FIRST_GLYPH];
    SDL_FRect box = { x, y, glyphW_, glyphH_ };
    SDL_SetTextureColorMod(glyph, color.r, color.g, color.b);
    SDL_RenderTexture(renderer, glyph, nullptr, &box);
}
