#include "Item.h"

namespace
{
    // One for each kind of item, in the same order as the enum
    constexpr ItemStats ITEM_STATS[] = {
        { "potion", '!', Palette::POTION },
        { "gold", '$', Palette::GOLD }
    };
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
