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

// The damage of each hit: more with a weapon than with bare hands
int Player::getAttack() const
{
    if (weapon_)
        return PLAYER_ATTACK + statsOf(*weapon_).attackBonus;
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

const std::vector<ItemKind>& Player::getInventory() const
{
    return inventory_;
}

std::optional<ItemKind> Player::getWeapon() const
{
    return weapon_;
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

// Heals, though never past full health
void Player::heal(int amount)
{
    hp_ = std::min(hp_ + amount, PLAYER_MAX_HP);
}

// Puts an item in the inventory, if there's room. Returns true if there was
bool Player::addToInventory(ItemKind kind)
{
    if (static_cast<int>(inventory_.size()) == INVENTORY_SIZE)
        return false;

    inventory_.push_back(kind);
    return true;
}

// Takes the item in a slot out of the inventory. The items after it move
// up a slot each
void Player::removeFromInventory(int slot)
{
    inventory_.erase(inventory_.begin() + slot);
}

// Takes up a weapon. The weapon it replaces, if there is one, goes into
// the inventory
void Player::wield(ItemKind weapon)
{
    if (weapon_)
        inventory_.push_back(*weapon_);
    weapon_ = weapon;
}

// Everything back to how it was at the start of the game
void Player::reset()
{
    hp_ = PLAYER_MAX_HP;
    gold_ = 0;
    inventory_.clear();
    weapon_ = std::nullopt;
}
