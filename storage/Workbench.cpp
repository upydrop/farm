#include "Workbench.hpp"
#include "../consts.hpp"
#include "Inventory.hpp"
#include "../sprite/Player.hpp"
#include "../items/Item.hpp"
#include "../GameMap.hpp"
#include <string>
#include <algorithm>


bool Workbench::recipesLoaded = false;

void Workbench::loadRecipes() {
    if (recipesLoaded) return;
    recipes.push_back({
        { "Wheat", "Wheat", "Wheat",
          "",      "",      "",
          "",      "",      "" },
        "Bread", 1
    });
    recipes.push_back({
        { "Wheat", "Wheat", "Wheat",
          "Carrot","Carrot","Carrot",
          "Wheat", "Wheat",  "Wheat" },
        "CarrotCake", 1
    });
    recipesLoaded = true;
}

Workbench::Workbench(float wx, float wy, ALLEGRO_BITMAP* bg, ALLEGRO_BITMAP* spr, ALLEGRO_FONT*   fnt): worldX(wx), worldY(wy), bgImage(bg), spriteImage(spr), font(fnt){
    loadRecipes();
    width = bgImage ? al_get_bitmap_width(bgImage) : 0;
    height = bgImage ? al_get_bitmap_height(bgImage) : 0;
}

Workbench::~Workbench(){
    for (auto& cell : craft_grid) {
        delete cell.item;
        cell.item     = nullptr;
        cell.quantity = 0;
    }
    delete result_item;
    result_item = nullptr;
}

Item* Workbench::buildResultItem(const std::string& name, int /*count*/) const
{
    if (name == "Bread")      return new Bread();
    if (name == "Wheat")      return new WheatItem();
    if (name == "Wheat Seed") return new WheatSeed();
    if (name == "Hoe")        return new Hoe();
    if (name == "Watering Can") return new WateringCan();
    if (name == "CarrotCake")   return new CarrotCake();
    return nullptr;
}

// after every grid change
void Workbench::updateResult()
{
    delete result_item;
    result_item  = nullptr;
    result_count = 0;

    for (const auto& recipe : recipes) {
        bool match = true;
        for (int i = 0; i < 9; ++i) {
            const std::string& expected = recipe.grid[i];
            const WorkbenchCell& cell   = craft_grid[i];

            if (expected.empty()) {
                if (!cell.isEmpty()) { match = false; break; }
            } else {
                if (cell.isEmpty() || cell.item->get_name() != expected)
                { match = false; break; }
            }
        }
        if (match) {
            result_item  = buildResultItem(recipe.resultName, recipe.resultCount);
            result_count = recipe.resultCount;
            return;
        }
    }
}

int Workbench::gridCellAtPoint(float sx, float sy) const{
    float gx = drawX + CRAFT_OFFSET_X;
    float gy = drawY + CRAFT_OFFSET_Y;

    if (sx < gx || sy < gy ||
        sx > gx + 3 * CELL_SIZE ||
        sy > gy + 3 * CELL_SIZE) return -1;

    int col = (int)((sx - gx) / CELL_SIZE);
    int row = (int)((sy - gy) / CELL_SIZE);
    if (col < 0 || col > 2 || row < 0 || row > 2) return -1;
    return row * 3 + col;
}

bool Workbench::resultSlotAtPoint(float sx, float sy) const{
    float rx = drawX + RESULT_OFFSET_X;
    float ry = drawY + RESULT_OFFSET_Y;
    return sx >= rx && sx <= rx + CELL_SIZE &&
           sy >= ry && sy <= ry + CELL_SIZE;
}

int Workbench::inventorySlotAtPoint(float sx, float sy, GameMap& gm) const{
    Inventory* inv = gm.player->get_inv();
    Point invPos   = inv->get_pos();
    int   sqSz     = inv->get_square_size();
    int   maxCount = inv->get_max_item_count();

    if (sx < invPos.x || sy < invPos.y ||
        sx > invPos.x + maxCount * sqSz ||
        sy > invPos.y + sqSz) return -1;

    int idx = (int)((sx - invPos.x) / sqSz);
    return (idx >= 0 && idx < maxCount) ? idx : -1;
}

void Workbench::update(GameMap& gm)
{
    if (open) {
        float dx = gm.player->get_pos()->x - worldX;
        float dy = gm.player->get_pos()->y - worldY;
        float distSq = dx*dx + dy*dy;
        if (distSq > INTERACT_RADIUS * INTERACT_RADIUS) {
            closeUI(gm);
        }

    }
}

void Workbench::returnCraftGridToInventory(GameMap& gm)
{
    Inventory* inv = gm.player->get_inv();
    for (auto& cell : craft_grid) {
        if (cell.isEmpty()) continue;

        if (!inv->isFull()) {
            inv->add_item(cell.item, cell.quantity); // add_item deletes the original and clones
        } else {
            cell.item->set_pos(*gm.player->get_pos());
            gm.dropped_loot.push_back({cell.item, cell.quantity});
        }
        cell.item     = nullptr;
        cell.quantity = 0;
    }
}

void Workbench::openUI(){
    open = true;

    // TODO - adjust width and height of workbench.png
    float bgW = bgImage ? al_get_bitmap_width(bgImage)  : 300.f;
    float bgH = bgImage ? al_get_bitmap_height(bgImage) : 200.f;
    drawX = (WIDTH  - bgW) / 2.f;
    drawY = (HEIGHT - bgH) / 2.f;
}

void Workbench::closeUI(GameMap& gm){
    returnCraftGridToInventory(gm);

    open = false;
}

bool Workbench::handleClick(float sx, float sy, bool rightButton, GameMap& gm){
    if (!open) return false;

    Inventory* inv = gm.player->get_inv();

    int cellIdx = gridCellAtPoint(sx, sy);
    if (cellIdx != -1) {
        WorkbenchCell& cell = craft_grid[cellIdx];

        if (gm.held_item == nullptr) {
            if (!cell.isEmpty()) {
                int toTake = rightButton ? (cell.quantity + 1) / 2 : cell.quantity;
                gm.held_item          = cell.item->clone();
                gm.held_item_quantity = toTake;
                cell.quantity -= toTake;
                if (cell.quantity <= 0) {
                    delete cell.item;
                    cell.item     = nullptr;
                    cell.quantity = 0;
                }
                updateResult();
            }
        } else {
            if (cell.isEmpty()) {
                int toPlace = rightButton ? 1 : gm.held_item_quantity;
                cell.item     = gm.held_item->clone();
                cell.quantity = toPlace;
                gm.held_item_quantity -= toPlace;
                if (gm.held_item_quantity <= 0) {
                    delete gm.held_item;
                    gm.held_item          = nullptr;
                    gm.held_item_quantity = 0;
                }
            } else if (cell.item->get_name() == gm.held_item->get_name() // stack into cell
                       && cell.item->isStackable()) {
                int toPlace = rightButton ? 1 : gm.held_item_quantity;
                cell.quantity += toPlace;
                gm.held_item_quantity -= toPlace;
                if (gm.held_item_quantity <= 0) {
                    delete gm.held_item;
                    gm.held_item          = nullptr;
                    gm.held_item_quantity = 0;
                }
            } else {
                // Swap
                Item* tmpItem     = cell.item;
                int   tmpQty      = cell.quantity;
                cell.item         = gm.held_item->clone();
                cell.quantity     = gm.held_item_quantity;
                delete gm.held_item;
                gm.held_item          = tmpItem;
                gm.held_item_quantity = tmpQty;
            }
            updateResult();
        }
        return true;
    }

    if (resultSlotAtPoint(sx, sy)) {
        if (result_item != nullptr && gm.held_item == nullptr) {
            inv->add_item(result_item->clone(), result_count);

            for (auto& cell : craft_grid) { // delete one of each ingredient
                if (cell.isEmpty()) continue;
                cell.quantity--;
                if (cell.quantity <= 0) {
                    delete cell.item;
                    cell.item     = nullptr;
                    cell.quantity = 0;
                }
            }
            updateResult();
        }
        return true;
    }

    int invIdx = inventorySlotAtPoint(sx, sy, gm);
    if (invIdx != -1) {
        InventorySlot* slot = inv->get_slot(invIdx);
        if (!slot) return false;

        if (gm.held_item == nullptr) {
            if (!slot->isEmpty()) {
                int toTake = rightButton ? (slot->count + 1) / 2 : slot->count;
                gm.held_item          = slot->item->clone();
                gm.held_item_quantity = toTake;
                slot->count -= toTake;
                if (slot->count <= 0) inv->set_item_to_zero(invIdx);
            }
        } else {
            if (slot->isEmpty()) {
                inv->add_item(gm.held_item->clone(), gm.held_item_quantity);
                delete gm.held_item;
                gm.held_item          = nullptr;
                gm.held_item_quantity = 0;
            } else if (slot->item->get_name() == gm.held_item->get_name()
                       && slot->item->isStackable()) {
                int toPlace = rightButton ? 1 : gm.held_item_quantity;
                slot->count += toPlace;
                gm.held_item_quantity -= toPlace;
                if (gm.held_item_quantity <= 0) {
                    delete gm.held_item;
                    gm.held_item          = nullptr;
                    gm.held_item_quantity = 0;
                }
            }
        }
        return true;
    }

    if (gm.held_item != nullptr) {
        gm.held_item->set_pos(*gm.player->get_pos());
        gm.dropped_loot.push_back({gm.held_item, gm.held_item_quantity});
        gm.held_item          = nullptr;
        gm.held_item_quantity = 0;
        return true;
    }

    return false;
}

void Workbench::drawSprite(float camX, float camY) const
{
    if (!spriteImage) return;
    float sx = worldX - camX;
    float sy = worldY - camY;
    al_draw_bitmap(spriteImage, sx, sy, 0);
}

void Workbench::draw(GameMap& gm) const{
    if (!open) return;

    if (bgImage) {
        al_draw_bitmap(bgImage, drawX, drawY, 0);
    } else {
        al_draw_filled_rectangle(drawX, drawY, drawX + 300, drawY + 200,
                                 al_map_rgb(80, 60, 40));
    }

    float gx = drawX + CRAFT_OFFSET_X;
    float gy = drawY + CRAFT_OFFSET_Y;

    for (int i = 0; i < 9; ++i) {
        int   col = i % 3;
        int   row = i / 3;
        float cx  = gx + col * CELL_SIZE;
        float cy  = gy + row * CELL_SIZE;

        const WorkbenchCell& cell = craft_grid[i];
        if (cell.isEmpty()) continue;

        cell.item->draw(Point{cx, cy}, CELL_SIZE, CELL_SIZE);

        if (cell.item->isStackable() && cell.quantity > 1 && font) {
            std::string qty = std::to_string(cell.quantity);
            al_draw_text(font, al_map_rgb(255,255,255), cx + 2, cy + 2, 0, qty.c_str());
        }
    }

    float rx = drawX + RESULT_OFFSET_X;
    float ry = drawY + RESULT_OFFSET_Y;

    if (result_item != nullptr) {
        result_item->draw(Point{rx, ry}, CELL_SIZE, CELL_SIZE);

        if (result_item->isStackable() && result_count > 1 && font) {
            std::string qty = std::to_string(result_count);
            al_draw_text(font, al_map_rgb(0,0,0),       rx + 3, ry + 3, 0, qty.c_str());
            al_draw_text(font, al_map_rgb(255, 215, 0), rx + 2, ry + 2, 0, qty.c_str());
        }
    }

    if (font) {
        al_draw_text(font, al_map_rgb(220, 200, 150),
                     drawX + width/2, drawY+10, ALLEGRO_ALIGN_CENTRE, "Workbench");
    }
}
