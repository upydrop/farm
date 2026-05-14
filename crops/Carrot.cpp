#include "Carrot.hpp"
#include "../items/Item.hpp"
#include <cstdio>

Carrot::Carrot(int tile_x, int tile_y): Crop(tile_x, tile_y, GROW_TIME, STAGES)
{

    loadSprites();
}

Carrot::~Carrot() {
    for (auto& bmp : sprites) {
        if (bmp) {
            al_destroy_bitmap(bmp);
            bmp = nullptr;
        }
    }
}

void Carrot::loadSprites() {
    const char* paths[STAGES] = {
        "images/crops/carrot/0.png", 
        "images/crops/carrot/1.png",  
        "images/crops/carrot/2.png",  
        "images/crops/carrot/3.png", 
    };

    for (int i = 0; i < STAGES; ++i) {
        ALLEGRO_BITMAP* bmp = al_load_bitmap(paths[i]);
        if (bmp) {
            sprites[i] = bmp;
        }
    }
}

std::vector<Item*> Carrot::getDrops() const {
    if (isMature())
        return { new CarrotSeed(), new CarrotSeed(), new CarrotItem(), new CarrotItem() };
    else
        return { new CarrotSeed() };
}

void Carrot::onStageChanged(int stage) {
    (void)stage;
}

void Carrot::onMature() {
    return;
}

void Carrot::onDeath() {
    return;
}