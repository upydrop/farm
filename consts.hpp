#ifndef IncludeConsts
#define IncludeConsts

#include "engine.hpp"
#include <iostream>
#include <src/tmxlite/Map.hpp>
#include <src/tmxlite/Layer.hpp>
#include <src/tmxlite/TileLayer.hpp>
#include <src/tmxlite/ObjectGroup.hpp>

#include <vector>

struct Point{
    float x,y;
};

struct tileType{
    int tileID;
    int tilesetIndex;
};

extern std::unordered_map<std::string, tileType> tileTypes;

const int TILESIZE = 16;
const int player_speed = 3;
const int player_speed_running = player_speed*1.5;
const int max_fatigue = 100.f; // player's
const float MAX_HEALTH = 6.f; // player's
const float MAX_HUNGER = 20.f; // player's; 0 = starving
const int WIDTH = Engine::WIDTH;
const int HEIGHT = Engine::HEIGHT;
const float fps = Engine::FPS;
const float dt = 1.0f / fps;
const double DAY_LENGTH = 10; //seconds - 900 for 15 mins TESTING
const double timeOfDay = 0.0;
const float ACTION_SPEED_ANIM = 30.0f; // скорость анимации действий (например, вспашки)
const float WALKING_SPEED_ANIM = 5.0f; // скорость анимации ходьбы
const float RUNNING_SPEED_ANIM = WALKING_SPEED_ANIM*1.5f;
const int LOOT_PICK_UP_RADIUS = 20;
const int INTERACT_RADIUS = 30;
const int CELL_SIZE = 43; // for workbench crafting grid and inventory slots

struct TileID{
    int layerID;
    int tileID;
};

struct Tileset {
    ALLEGRO_BITMAP* texture = nullptr;
    int tileW = 0, tileH = 0;
    int columns  = 0;
    int firstGID = 0;
    int lastGID  = 0;
};

struct Tile {
    int id           = -1;
    int tilesetIndex = -1;
};

struct CollisionRect {
    Point pos;
    float w, h;
};


#endif