#pragma once
#include "Entity.h"

// The kinds of treasure
enum class ItemKind
{
    Potion,
    Gold,
    MagicMapping,
    Dagger,
    Sword,
    Warhammer,
    Fireball
};

constexpr int ITEM_KINDS = 7;   // how many kinds there are

// What every item of one kind is like
struct ItemStats
{
    const char* name;
    char glyph;
    SDL_Color color;
    int attackBonus = 0;   // for a weapon, the extra damage of each hit
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
