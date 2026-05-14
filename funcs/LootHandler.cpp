#include "LootHandler.hpp"
#include <algorithm>
#include <cmath>
#include "../crops/Crop.hpp"
#include "../items/Item.hpp"
#include "../sprite/Player.hpp"

void harvestCropAtPosition(GameMap& gmap, float worldX, float worldY) {
    gmap.crops.erase(
        std::remove_if(gmap.crops.begin(), gmap.crops.end(),
            [&](const std::unique_ptr<Crop>& c) {
                float cx = c->getTileX() * TILESIZE;
                float cy = c->getTileY() * TILESIZE;
                if (worldX < cx || worldX >= cx + TILESIZE ||
                    worldY < cy || worldY >= cy + TILESIZE)
                    return false;

                Point* P_pos = gmap.player->get_pos();
                float tileCX = cx + TILESIZE / 2.f;
                float tileCY = cy + TILESIZE / 2.f;
                float playerCX = P_pos->x + gmap.player->get_w() / 2.f;
                float playerCY = P_pos->y + gmap.player->get_h() / 2.f;
                float dist = sqrt(pow(tileCX - playerCX, 2) + pow(tileCY - playerCY, 2));
                if (dist > LOOT_PICK_UP_RADIUS) return false;

                for (Item* drop : c->getDrops()) {
                    drop->set_pos(Point{ cx, cy });
                    gmap.dropped_loot.push_back({drop, 1});
                }
                return true;
            }),
        gmap.crops.end()
    );
}