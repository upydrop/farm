#ifndef IncludeGameMap
#define IncludeGameMap
#include "consts.hpp"

class Sprite;
class Player;
class Item;
class Crop;

using namespace std;

struct GameMap {
    int mapW = 0, mapH = 0; 
    int tileW = 0, tileH = 0;

    std::vector<Tileset>                        tilesets;
    std::vector<std::vector<Tile>>              tileLayers;  
    std::vector<CollisionRect>                  collisions;    
    std::vector<Sprite*>                        sprites;
    std::vector<Sprite*>                        collisionSprites;
    std::vector<Item*>                          dropped_loot;
    std::vector<std::unique_ptr<Crop>>          crops; //посаженный урожай
    Player*                player = nullptr;
    ALLEGRO_BITMAP*        mapBuffer = nullptr;
    Point                  mouseWorldPos;
};

#endif