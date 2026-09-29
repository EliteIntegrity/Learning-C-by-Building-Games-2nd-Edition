#include "SaveLoad.h"
#include <fstream>   // std::ofstream and std::ifstream
#include "Enemy.h"
#include "Item.h"
#include "Map.h"
#include "Player.h"

// The first word of every save file, and the version of the format that
// follows it. The version goes up whenever the format changes, so that an
// old file is turned away, rather than read wrongly
const std::string SAVE_HEADER = "ROGUE_SDL_SAVE";
constexpr int SAVE_VERSION = 1;

namespace
{
    // A tile as one letter: W for wall, F for floor, and S for stairs, as
    // a capital if the player has explored it
    char encode(const Tile& tile)
    {
        switch (tile.terrain)
        {
        case Terrain::Floor:
            return tile.explored ? 'F' : 'f';
        case Terrain::StairsDown:
            return tile.explored ? 'S' : 's';
        default:
            return tile.explored ? 'W' : 'w';
        }
    }

    // The tile a letter stands for. Anything but F or S is wall
    Tile decode(char letter)
    {
        Tile tile;
        tile.explored = letter >= 'A' && letter <= 'Z';
        if (letter == 'F' || letter == 'f')
            tile.terrain = Terrain::Floor;
        else if (letter == 'S' || letter == 's')
            tile.terrain = Terrain::StairsDown;
        return tile;
    }
}

namespace SaveLoad
{
    // Writes everything the game needs to carry on later: the depth, the
    // player, the monsters, the treasure, and the map. Returns false if the
    // file couldn't be written
    bool save(const std::string& path, const Map& map, const Player& player,
              const std::vector<Enemy>& enemies,
              const std::vector<Item>& items, int depth)
    {
        std::ofstream file(path);
        file << SAVE_HEADER << " " << SAVE_VERSION << "\n";
        file << "DEPTH " << depth << "\n";

        Point at = player.getPosition();
        file << "PLAYER " << at.x << " " << at.y << " " << player.getHp()
             << " " << player.getGold() << " " << player.getPotions() << "\n";

        for (const Enemy& enemy : enemies)
        {
            at = enemy.getPosition();
            file << "MONSTER " << static_cast<int>(enemy.getKind()) << " "
                 << at.x << " " << at.y << " " << enemy.getHp() << "\n";
        }
        for (const Item& item : items)
        {
            at = item.getPosition();
            file << "ITEM " << static_cast<int>(item.getKind()) << " "
                 << at.x << " " << at.y << " " << item.getAmount() << "\n";
        }

        // The map last, a row of letters to a line
        file << "MAP\n";
        for (int y = 0; y < MAP_H; ++y)
        {
            for (int x = 0; x < MAP_W; ++x)
                file << encode(map.at({ x, y }));
            file << "\n";
        }

        // Closing the file finishes the writing, so any problem shows now
        file.close();
        return !file.fail();
    }

    // Reads a saved game back into the game's variables. If anything in
    // the file is missing or wrong, it returns false, and leaves the game
    // exactly as it was
    bool load(const std::string& path, Map& map, Player& player,
              std::vector<Enemy>& enemies, std::vector<Item>& items,
              int& depth)
    {
        std::ifstream file(path);
        std::string word;
        int version = 0;
        file >> word >> version;
        if (word != SAVE_HEADER || version != SAVE_VERSION)
            return false;

        // Read everything into new variables first
        int newDepth = 1;
        Player newPlayer;
        std::vector<Enemy> newEnemies;
        std::vector<Item> newItems;
        Map newMap;

        // A line at a time, each starting with a word that says what it is,
        // until the map
        while (file >> word && word != "MAP")
        {
            Point at;
            if (word == "DEPTH")
            {
                file >> newDepth;
            }
            else if (word == "PLAYER")
            {
                int hp = 0;
                int gold = 0;
                int potions = 0;
                file >> at.x >> at.y >> hp >> gold >> potions;
                newPlayer.setPosition(at);
                newPlayer.setHp(hp);
                newPlayer.setGold(gold);
                newPlayer.setPotions(potions);
            }
            else if (word == "MONSTER")
            {
                int kind = 0;
                int hp = 0;
                file >> kind >> at.x >> at.y >> hp;
                if (kind < 0 || kind >= MONSTER_KINDS)
                    return false;

                Enemy enemy(static_cast<MonsterKind>(kind), at);
                enemy.setHp(hp);
                newEnemies.push_back(enemy);
            }
            else if (word == "ITEM")
            {
                int kind = 0;
                int amount = 0;
                file >> kind >> at.x >> at.y >> amount;
                if (kind < 0 || kind >= ITEM_KINDS)
                    return false;

                newItems.push_back(Item(static_cast<ItemKind>(kind), at,
                                        amount));
            }
            else
            {
                return false;   // a word that has no business here
            }

            // A place outside the map would crash the game when it's drawn
            if (!newMap.isInside(at))
                return false;
        }

        // The map, which must be a full MAP_W letters by MAP_H lines
        for (int y = 0; y < MAP_H; ++y)
        {
            std::string row;
            file >> row;
            if (static_cast<int>(row.size()) != MAP_W)
                return false;

            for (int x = 0; x < MAP_W; ++x)
                newMap.at({ x, y }) = decode(row[x]);
        }

        // The whole file was good, so now the game can have it
        map = newMap;
        player = newPlayer;
        enemies = newEnemies;
        items = newItems;
        depth = newDepth;
        return true;
    }
}
