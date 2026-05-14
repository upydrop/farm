#include "GameMap.hpp"
#include "storage/Inventory.hpp"
#include "items/Item.hpp"
#include "sprite/Player.hpp"
#include "crops/Crop.hpp"

void GameMap::dropHeldItem(){
    if (!held_item) return;
    held_item->set_pos(player ? *player->get_pos() : Point{0,0});
    dropped_loot.push_back({held_item, held_item_quantity});
    held_item          = nullptr;
    held_item_quantity = 0;
}

// Drop held_item back to inventory, or to ground if full.
void GameMap::returnHeldItemToInventory()
{
    if (!held_item) return;
    Inventory* inv = player->get_inv();
    if (!inv->isFull()) {
        inv->add_item(held_item, held_item_quantity); // add_item deletes original
    } else {
        held_item->set_pos(*player->get_pos());
        dropped_loot.push_back({held_item, held_item_quantity});
    }
    held_item          = nullptr;
    held_item_quantity = 0;
}

GameMap::~GameMap() = default;
