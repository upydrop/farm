#include "FishingRod.hpp"
#include "../sprite/Player.hpp"
#include "../funcs/mapFuncs.hpp"

void FishingRod::calcRodTip(GameMap& gmap) {
    Point* p = gmap.player->get_pos();
    // Кончик удочки — правый верхний угол спрайта
    rodTipX = p->x + gmap.player->get_w() + get_w() - 4.f;
    rodTipY = p->y + 4.f;
}

void FishingRod::spawnLoot(GameMap& gmap) {
    if (!lootFactory) return;
    Item* drop = lootFactory(gmap);
    if (!drop) return;
    drop->set_pos(*gmap.player->get_pos());
    gmap.dropped_loot.push_back({drop, 1});
}

void FishingRod::retract() {
    fishState      = FishingState::Idle;
    bobber.inWater = false;
    reelTimer      = 0.f;
    waitTimer      = 0.f;
}

bool FishingRod::functionality(GameMap& gmap) {
    if (fishState != FishingState::Idle) {
        retract();
        return false;
    };
    Point pos_ = gmap.mouseWorldPos;
    vector<TileID> tileIDs = getTileIndex(gmap, floor(pos_.x/TILESIZE), floor(pos_.y/TILESIZE));
    if (tileIDs.size() <= 0) return false;
    for (const auto& tileID : tileIDs){
        Tile& tile = gmap.tileLayers[tileID.layerID][tileID.tileID];
        tileType water = tileTypes["water"];
        tileType water1 = tileTypes["water1"];
        tileType water2 = tileTypes["water2"];
        if (tile.id == water.tileID && tile.tilesetIndex == water.tilesetIndex || tile.id == water1.tileID && tile.tilesetIndex == water1.tilesetIndex || tile.id == water2.tileID && tile.tilesetIndex == water2.tilesetIndex){
            calcRodTip(gmap);

            bobber.x       = rodTipX;
            bobber.y       = rodTipY;
            bobber.targetX = pos_.x;
            bobber.targetY = pos_.y;
            bobber.inWater = false;

            fishState = FishingState::Casting;
            change_healthPoints(-1);
            return false;
        }
    }
    return false;
}

void FishingRod::update(GameMap& gmap) {
    if (fishState == FishingState::Idle) return;

    calcRodTip(gmap);

    // Авто-убирание если игрок отошёл слишком далеко от поплавка
    if (fishState != FishingState::Reeling) {
        Point* p = gmap.player->get_pos();
        float dx = bobber.targetX - (p->x + gmap.player->get_w() * 0.5f);
        float dy = bobber.targetY - (p->y + gmap.player->get_h() * 0.5f);
        if (std::sqrt(dx*dx + dy*dy) > autoRetractRadius) {
            retract();
            return;
        }
    }

    switch (fishState) {

    case FishingState::Casting: {
        float dx   = bobber.targetX - bobber.x;
        float dy   = bobber.targetY - bobber.y;
        float dist = std::sqrt(dx*dx + dy*dy);

        if (dist < 4.f) {
            bobber.x = bobber.targetX;
            bobber.y = bobber.targetY;
            bobber.inWater = true;
            waitDuration   = 2.f + (std::rand() % 400) / 100.f;
            waitTimer      = 0.f;
            fishState      = FishingState::Waiting;
        } else {
            float step = bobber.speed * dt;
            bobber.x  += (dx / dist) * step;
            bobber.y  += (dy / dist) * step;
        }
        break;
    }

    case FishingState::Waiting: {
        waitTimer += dt;
        if (waitTimer >= waitDuration) {
            spawnLoot(gmap);
            reelTimer = 0.f;
            fishState = FishingState::Reeling;
        }
        break;
    }

    case FishingState::Reeling: {
        float dx   = rodTipX - bobber.x;
        float dy   = rodTipY - bobber.y;
        float dist = std::sqrt(dx*dx + dy*dy);

        reelTimer += dt;
        if (dist < 6.f || reelTimer >= reelDuration) {
            retract();
        } else {
            float step = bobber.speed * 1.5f * dt;
            bobber.x  += (dx / dist) * step;
            bobber.y  += (dy / dist) * step;
        }
        break;
    }

    default: break;
    }
}

void FishingRod::draw(Point pos_, int new_w, int new_h, std::optional<std::reference_wrapper<GameMap>> gmap) {
    if (fishState == FishingState::Idle) {
        Item::draw(pos_, new_w, new_h);
        return;
    }
    float offsetY = -HEIGHT/2;
    float offsetX = -WIDTH/2;
    if (gmap){
        GameMap& map = gmap->get();
        offsetY = -(map.player->get_pos()->y - HEIGHT/2);
        offsetX = -(map.player->get_pos()->x - WIDTH/2);
    };
    // Удочка на игроке
    Item::draw(Point{rodTipX - get_w() + offsetX, rodTipY + offsetY}, new_w, new_h);

    // Леска от кончика до верхней середины поплавка
    al_draw_line(rodTipX + offsetX, rodTipY+offsetY,
                 bobber.x + offsetX , bobber.y + offsetY,
                 al_map_rgb(0, 0, 0), 1.0f);

    // Поплавок картинка, привязанная верхней серединой к bobber.x/y
    if (bobberImg) {
        float bw = al_get_bitmap_width(bobberImg);
        float bh = al_get_bitmap_height(bobberImg);
        float drawX = bobber.x - bw * 0.5f;
        float drawY = bobber.y;
        al_draw_bitmap(bobberImg, drawX + offsetX, drawY + offsetY, 0);
    }
}