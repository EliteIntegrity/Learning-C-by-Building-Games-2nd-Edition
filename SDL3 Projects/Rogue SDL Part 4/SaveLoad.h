#pragma once
#include <string>   // std::string, for the file's path
#include <vector>   // std::vector, for the monsters and the treasure

class Enemy;
class Item;
class Map;
class Player;

// Saving the game to a text file, and loading it back
namespace SaveLoad
{
    bool save(const std::string& path, const Map& map, const Player& player,
              const std::vector<Enemy>& enemies,
              const std::vector<Item>& items, int depth);
    bool load(const std::string& path, Map& map, Player& player,
              std::vector<Enemy>& enemies, std::vector<Item>& items,
              int& depth);
}
