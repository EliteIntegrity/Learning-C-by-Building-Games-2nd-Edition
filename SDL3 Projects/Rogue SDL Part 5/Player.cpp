#include "Player.h"
#include <algorithm>   // std::min and std::max
#include "Map.h"

Player::Player()
    : Entity({ 0, 0 }, '@', Palette::PLAYER)
{
}

// Steps one cell, unless the way is blocked. Returns true if the player
// moved
bool Player::tryMove(int dx, int dy, const Map& map)
{
    Point next = { getPosition().x + dx, getPosition().y + dy };
    if (map.isBlocked(next))
        return false;

    setPosition(next);
    return true;
}

int Player::getHp() const
{
    return hp_;
}

void Player::setHp(int hp)
{
    hp_ = hp;
}

int Player::getAttack() const
{
    return PLAYER_ATTACK;
}

int Player::getGold() const
{
    return gold_;
}

void Player::setGold(int gold)
{
    gold_ = gold;
}

int Player::getPotions() const
{
    return potions_;
}

void Player::setPotions(int potions)
{
    potions_ = potions;
}

bool Player::isAlive() const
{
    return hp_ > 0;
}

// Never below zero, so that the HUD never shows a negative number
void Player::takeDamage(int amount)
{
    hp_ = std::max(hp_ - amount, 0);
}

void Player::addGold(int amount)
{
    gold_ += amount;
}

void Player::addPotion()
{
    ++potions_;
}

// Drinks a potion, if there's one to drink, and heals, though never past
// full health. Returns true if a potion was drunk
bool Player::drinkPotion()
{
    if (potions_ == 0)
        return false;

    --potions_;
    hp_ = std::min(hp_ + POTION_HEAL, PLAYER_MAX_HP);
    return true;
}

// Everything back to how it was at the start of the game
void Player::reset()
{
    hp_ = PLAYER_MAX_HP;
    gold_ = 0;
    potions_ = 0;
}
