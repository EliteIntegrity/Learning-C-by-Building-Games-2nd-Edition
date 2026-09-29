#include "Enemy.h"
#include <iterator>   // std::size

namespace
{
    // One for each kind of monster, in the same order as the enum
    constexpr MonsterStats MONSTER_STATS[] = {
        { "rat", 'r', Palette::RAT, 4, 2, 6 },
        { "goblin", 'g', Palette::GOBLIN, 8, 3, 7 },
        { "orc", 'o', Palette::ORC, 14, 5, 8 }
    };
    static_assert(std::size(MONSTER_STATS) == MONSTER_KINDS,
                  "There must be one MonsterStats for each MonsterKind");
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

MonsterKind Enemy::getKind() const
{
    return kind_;
}

const MonsterStats& Enemy::getStats() const
{
    return statsOf(kind_);
}

int Enemy::getHp() const
{
    return hp_;
}

void Enemy::setHp(int hp)
{
    hp_ = hp;
}

bool Enemy::isAlive() const
{
    return hp_ > 0;
}

void Enemy::takeDamage(int amount)
{
    hp_ -= amount;
}

bool Enemy::isHunting() const
{
    return hunting_;
}

Point Enemy::getLastSeen() const
{
    return lastSeen_;
}

// Starts hunting the player, or keeps on hunting, with a fresh sighting
void Enemy::hunt(Point lastSeen)
{
    hunting_ = true;
    lastSeen_ = lastSeen;
}

// Loses the trail, and waits where it is
void Enemy::giveUp()
{
    hunting_ = false;
}
