#pragma once
#include "Entity.h"

// The kinds of treasure
enum class ItemKind
{
    Potion,
    Gold
};

// What every item of one kind is like
struct ItemStats
{
    const char* name;
    char glyph;
    SDL_Color color;
};

const ItemStats& statsOf(ItemKind kind);

// Something lying on the floor, waiting to be picked up
class Item : public Entity
{
public:
    Item(ItemKind kind, Point position, int amount = 1);

    ItemKind getKind() const;
    int getAmount() const;

private:
    ItemKind kind_;
    int amount_;   // how many coins, for gold
};
