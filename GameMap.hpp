#ifndef IncludeGameMap
#define IncludeGameMap
#include "consts.hpp"
#include <memory>
#include "storage/Workbench.hpp"

class Sprite;
class Player;
class Item;
class Crop;

using namespace std;

struct DroppedLoot {
    Item* item;
    int quantity;
};

struct GameMap {
    int mapW = 0, mapH = 0; 
    int tileW = 0, tileH = 0;

    std::vector<Tileset>                        tilesets;
    std::vector<std::vector<Tile>>              tileLayers;  
    std::vector<CollisionRect>                  collisions;    
    std::vector<Sprite*>                        sprites;
    std::vector<Sprite*>                        collisionSprites;
    std::vector<DroppedLoot>                    dropped_loot; // std::vector<dropped_loot>
    std::vector<std::unique_ptr<Crop>>          crops; //посаженный урожай
    std::vector<std::unique_ptr<Workbench>>     worktables;
    Player*                                     player = nullptr;
    ALLEGRO_BITMAP*                             mapBuffer = nullptr;
    Point                                       mouseWorldPos;

    // Always delete before nulling.
    // Drawn at cursor position.
    Item* held_item             = nullptr;
    int   held_item_quantity    = 0;
 
    void dropHeldItem();
 
    // Return held_item to inventory; drop if full.
    void returnHeldItemToInventory(); 

    ~GameMap();
};

#endif