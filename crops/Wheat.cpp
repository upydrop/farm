#include "Wheat.hpp"
#include "../items/Item.hpp"
#include <allegro5/allegro_image.h>
#include <cstdio>

Wheat::Wheat(int tile_x, int tile_y): Crop(tile_x, tile_y, GROW_TIME, STAGES)
{

    loadSprites();
}

Wheat::~Wheat() {
    for (auto& bmp : sprites) {
        if (bmp) {
            al_destroy_bitmap(bmp);
            bmp = nullptr;
        }
    }
}

void Wheat::loadSprites() {
    const char* paths[STAGES] = {
        "images/crops/wheat/0.png", 
        "images/crops/wheat/1.png",  
        "images/crops/wheat/2.png",  
        "images/crops/wheat/3.png", 
    };

    for (int i = 0; i < STAGES; ++i) {
        ALLEGRO_BITMAP* bmp = al_load_bitmap(paths[i]);
        if (bmp) {
            sprites[i] = bmp;
        }
    }
}

std::vector<Item*> Wheat::getDrops() const {
    if (isMature())
        return { new WheatSeed(), new WheatSeed(), new WheatItem(), new WheatItem() };
    else
        return { new WheatSeed() };
}

void Wheat::onStageChanged(int stage) {
    (void)stage;
}

void Wheat::onMature() {
    return;
}

void Wheat::onDeath() {
    return;
}