#include "Enemy.h"

namespace
{
    // One for each kind of monster, in the same order as the enum
    constexpr MonsterStats MONSTER_STATS[] = {
        { "rat", 'r', Palette::RAT, 4, 2, 6 },
        { "goblin", 'g', Palette::GOBLIN, 8, 3, 7 },
        { "orc", 'o', Palette::ORC, 14, 5, 8 }
    };
}

const MonsterStats& statsOf(MonsterKind kind)
{
    return MONSTER_STATS[static_cast<int>(kind)];
}

Enemy::Enemy(MonsterKind kind, Point position)
    : Entity(position, statsOf(kind).glyph, statsOf(kind).color),
      kind_(kind),
      hp_(statsOf(kind).maxHp)
{
}

const MonsterStats& Enemy::getStats() const
{
    return statsOf(kind_);
}

int Enemy::getHp() const
{
    return hp_;
}

bool Enemy::isAlive() const
{
    return hp_ > 0;
}

void Enemy::takeDamage(int amount)
{
    hp_ -= amount;
}
