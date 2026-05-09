#include "mapFuncs.hpp"
#include <algorithm>
#include "../GameMap.hpp"

int findTilesetIndex(const GameMap& gmap, uint32_t gid) {
    for (int i = 0; i < (int)gmap.tilesets.size(); i++)
        if (gid >= (uint32_t)gmap.tilesets[i].firstGID &&
            gid <= (uint32_t)gmap.tilesets[i].lastGID)
            return i;
    return -1;
}

vector<TileID> getTileIndex(GameMap& gmap, int tileX, int tileY){
    vector<TileID> result;
    if (tileX < 0 || tileX >= gmap.mapW || tileY < 0 || tileY >= gmap.mapH){
        return result;
    }
    int index = tileY * gmap.mapW + tileX;
    for (int layerID = 0; layerID < gmap.tileLayers.size(); ++layerID){
        if (index < gmap.tileLayers[layerID].size()){
            result.push_back(TileID{layerID, index});
        }
    }
    return result;
}

bool deleteTile(GameMap& gmap, int tileLayer, int tileIndex) { 
    gmap.tileLayers[tileLayer][tileIndex].id = -1;
    gmap.tileLayers[tileLayer][tileIndex].tilesetIndex = -1;
    return true;
}

bool setTile(GameMap& gmap, int tileLayer, int tileIndex, int new_tileID, int new_tilesetID){
    gmap.tileLayers[tileLayer][tileIndex].id = new_tileID;
    gmap.tileLayers[tileLayer][tileIndex].tilesetIndex = new_tilesetID;
    return true;
}

bool update_buffer(GameMap& gmap){
    ALLEGRO_BITMAP* prevTarget = al_get_target_bitmap();
    al_set_target_bitmap(gmap.mapBuffer);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));

    for (int t = 0; t < gmap.tileLayers.size(); ++t){
        for (int y = 0; y < gmap.mapH; y++) {
            for (int x = 0; x < gmap.mapW; x++) {
                const Tile& tile = gmap.tileLayers[t][y * gmap.mapW + x];
                if (tile.id < 0 || tile.tilesetIndex < 0) continue;

                const Tileset& ts = gmap.tilesets[tile.tilesetIndex];
                int srcX = (tile.id % ts.columns) * ts.tileW;
                int srcY = (tile.id / ts.columns) * ts.tileH;

                al_draw_bitmap_region(ts.texture,
                    srcX, srcY, ts.tileW, ts.tileH,
                    x * gmap.tileW, y * gmap.tileH, 0);
            }
        }
    }

    al_set_target_bitmap(prevTarget);
    return true;
}

bool loadMap(GameMap& gmap, const std::string& path) {
    tmx::Map tmxMap;
    if (!tmxMap.load(path)) return false;

    gmap.mapW  = tmxMap.getTileCount().x;
    gmap.mapH  = tmxMap.getTileCount().y;
    gmap.tileW = tmxMap.getTileSize().x;
    gmap.tileH = tmxMap.getTileSize().y;

    for (const auto& ts : tmxMap.getTilesets()) {
        Tileset t;
        t.firstGID = ts.getFirstGID();
        t.lastGID  = ts.getLastGID();
        t.tileW    = ts.getTileSize().x;
        t.tileH    = ts.getTileSize().y;
        t.columns  = ts.getColumnCount();
        t.texture  = al_load_bitmap(ts.getImagePath().c_str());
        gmap.tilesets.push_back(t);
    }

    for (const auto& layer : tmxMap.getLayers()) {
        const std::string& name = layer->getName();

        if (layer->getType() == tmx::Layer::Type::Tile) {
            const auto& src = layer->getLayerAs<tmx::TileLayer>();
            const auto& tiles = src.getTiles();
            
            std::vector<Tile> currentLayerTiles(gmap.mapW * gmap.mapH);

            for (int i = 0; i < (int)tiles.size(); i++) {
                uint32_t gid = tiles[i].ID;
                if (gid == 0) {
                    currentLayerTiles[i].id = -1;
                    currentLayerTiles[i].tilesetIndex = -1;
                    continue;
                }
                gid &= ~(0x80000000 | 0x40000000 | 0x20000000);
                int ti = findTilesetIndex(gmap, gid);
                if (ti < 0) continue;
                currentLayerTiles[i].tilesetIndex = ti;
                currentLayerTiles[i].id = gid - gmap.tilesets[ti].firstGID;
            }
            gmap.tileLayers.push_back(currentLayerTiles);
        }

        else if (name == "Collisions" && layer->getType() == tmx::Layer::Type::Object) {
            for (const auto& obj : layer->getLayerAs<tmx::ObjectGroup>().getObjects()) {
                CollisionRect r;
                r.pos.x = obj.getPosition().x;
                r.pos.y = obj.getPosition().y;
                r.w = obj.getAABB().width;
                r.h = obj.getAABB().height;
                gmap.collisions.push_back(r);
            }
        }

        else if (name == "Objects" && layer->getType() == tmx::Layer::Type::Object) {
            for (const auto& obj : layer->getLayerAs<tmx::ObjectGroup>().getObjects()) {
                Sprite* s = new Sprite(obj.getName(), Point{obj.getPosition().x, obj.getPosition().y}, nullptr, obj.getAABB().width, obj.getAABB().height);
                bool solid = obj.getType() == "Solid";

                uint32_t gid = obj.getTileID();
                if (gid != 0) {
                    gid &= ~(0x80000000 | 0x40000000 | 0x20000000);
                    int ti = findTilesetIndex(gmap, gid);
                    if (ti >= 0) {
                        s->set_tilesetIndex(ti);
                        s->set_tileID(gid - gmap.tilesets[ti].firstGID);
                    }
                }

                if (solid)
                    gmap.collisionSprites.push_back(s);
                else
                    gmap.sprites.push_back(s);
            }
        }

        else if (name == "Entities" && layer->getType() == tmx::Layer::Type::Object) {
            for (const auto& obj : layer->getLayerAs<tmx::ObjectGroup>().getObjects()) {
                if (obj.getName() == "Player"){
                    ALLEGRO_BITMAP* bitmap = al_load_bitmap("images/player/walking/down/0.png");
                    Player* s = new Player(obj.getName(), Point{obj.getPosition().x, obj.getPosition().y}, bitmap, al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap));
                    gmap.player = s;
                    continue;
                }
            }
        }
    }

    gmap.mapBuffer = al_create_bitmap(gmap.mapW * gmap.tileW,
                                      gmap.mapH * gmap.tileH);
    update_buffer(gmap);

    return true;
}