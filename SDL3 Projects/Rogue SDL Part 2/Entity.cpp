#include "Entity.h"
#include "GlyphCache.h"

Entity::Entity(Point position, char glyph, SDL_Color color)
    : position_(position), glyph_(glyph), color_(color)
{
}

Point Entity::getPosition() const
{
    return position_;
}

void Entity::setPosition(Point position)
{
    position_ = position;
}

void Entity::draw(SDL_Renderer* renderer, const GlyphCache& glyphs) const
{
    glyphs.draw(renderer, glyph_, position_, color_);
}
