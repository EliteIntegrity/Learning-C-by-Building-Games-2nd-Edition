#pragma once
#include <SDL3/SDL.h>
#include <array>   // std::array, for the glyphs
#include <string>   // std::string, for the font's path
#include "Common.h"

// The printable characters, from the space (code 32) to the tilde (126)
constexpr char FIRST_GLYPH = ' ';
constexpr char LAST_GLYPH = '~';
constexpr int GLYPH_COUNT = LAST_GLYPH - FIRST_GLYPH + 1;

// Every character the game can draw, made once from a font as a white
// picture, and tinted to whatever color it's drawn in
class GlyphCache
{
public:
    GlyphCache(SDL_Renderer* renderer, const std::string& fontPath,
               float size);
    ~GlyphCache();

    GlyphCache(const GlyphCache&) = delete;
    GlyphCache& operator=(const GlyphCache&) = delete;

    bool isLoaded() const;
    void draw(SDL_Renderer* renderer, char c, Point cell,
              SDL_Color color) const;
    void drawText(SDL_Renderer* renderer, const std::string& text,
                  Point cell, SDL_Color color) const;

private:
    void drawAt(SDL_Renderer* renderer, char c, float x, float y,
                SDL_Color color) const;

    std::array<SDL_Texture*, GLYPH_COUNT> glyphs_ = {};
    float glyphW_ = 0.0f;   // the size of every glyph, since the font is
    float glyphH_ = 0.0f;   // monospaced
    bool loaded_ = false;
};
