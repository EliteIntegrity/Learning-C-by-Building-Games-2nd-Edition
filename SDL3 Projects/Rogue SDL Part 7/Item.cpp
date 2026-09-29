#include "Item.h"
#include <iterator>   // std::size

namespace
{
    // One for each kind of item, in the same order as the enum
    constexpr ItemStats ITEM_STATS[] = {
        { "potion of healing", '!', Palette::POTION },
        { "gold", '$', Palette::GOLD },
        { "scroll of magic mapping", '?', Palette::SCROLL },
        { "dagger", ')', Palette::WEAPON, 2 },
        { "sword", ')', Palette::WEAPON, 5 },
        { "warhammer", ')', Palette::WEAPON, 10 },
        { "scroll of fireball", '?', Palette::SCROLL }
    };
    static_assert(std::size(ITEM_STATS) == ITEM_KINDS,
                  "There must be one ItemStats for each ItemKind");
}

const ItemStats& statsOf(ItemKind kind)
{
    return ITEM_STATS[static_cast<int>(kind)];
}

Item::Item(ItemKind kind, Point position, int amount)
    : Entity(position, statsOf(kind).glyph, statsOf(kind).color),
      kind_(kind),
      amount_(amount)
{
}

ItemKind Item::getKind() const
{
    return kind_;
}

int Item::getAmount() const
{
    return amount_;
}
