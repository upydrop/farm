#ifndef Include_MAP_LOADER
#define Include_MAP_LOADER
#include "../sprite/Player.hpp"

vector<TileID> getTileIndex(GameMap& gmap, int tileX, int tileY);
bool deleteTile(GameMap& gmap, int tilesetIndex, int tileIndex);
bool setTile(GameMap& gmap, int tileLayer, int tileIndex, int new_tileID, int new_tilesetID);
bool update_buffer(GameMap& gmap);
bool loadMap(GameMap& gmap, const std::string& filename);

#endif