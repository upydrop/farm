#include "Bag.hpp"
#include "../consts.hpp"
#include "../GameMap.hpp"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <algorithm>
#include <string>

const int Bag::COLS;
const int Bag::ROWS;
const int Bag::CELL_SIZE;
const int Bag::MAX_STACK;

Bag::Bag(ALLEGRO_BITMAP* img, ALLEGRO_FONT* fnt): gridImage(img), font(fnt){
    cells.resize(COLS * ROWS);

    // Centre the bag window on screen
    w = al_get_bitmap_width(img);
    h = al_get_bitmap_height(img);
    const float totalW = w;
    const float totalH = h;
    drawX = (WIDTH  - totalW) / 2.f;
    drawY = (HEIGHT - totalH) / 2.f;
}

Bag::~Bag(){
    for (auto& c : cells) {
        delete c.item;
        c.item = nullptr;
    }
}


bool Bag::canStack(Item* a, Item* b) const{
    if (!a || !b)             return false;
    if (!a->isStackable())    return false;
    if (a->get_name() != b->get_name()) return false;
    return true;
}

int Bag::findStackSlot(Item* item) const{
    for (int i = 0; i < (int)cells.size(); ++i) {
        const auto& c = cells[i];
        if (!c.isEmpty() && canStack(c.item, item) && c.quantity < MAX_STACK)
            return i;
    }
    return -1;
}

int Bag::findFreeSlot() const{
    for (int i = 0; i < (int)cells.size(); ++i)
        if (cells[i].isEmpty()) return i;
    return -1;
}

int Bag::halfOf(int count) const
{
    return (count + 1) / 2;
}

int Bag::cellAtPoint(float sx, float sy) const{
    //transforming screen coords(sx,sy) into bag coords to compare with gridOffset's
    sx -= drawX;
    sy -= drawY;

    float gridW = COLS * CELL_SIZE;
    float gridH = ROWS * CELL_SIZE;

    if (sx < gridOffsetX || sy < gridOffsetY || sx > gridOffsetX + gridW || sy > gridOffsetY + gridH) return -1;

    int col = (int)((sx - gridOffsetX) / CELL_SIZE);
    int row = (int)((sy - gridOffsetY) / CELL_SIZE);

    if (col < 0 || col >= COLS || row < 0 || row >= ROWS) return -1;
    return row * COLS + col;
}


bool Bag::addItem(Item* item, int quantity){
    int remaining = quantity;

    if (item->isStackable()) {
        while (remaining > 0) {
            int idx = findStackSlot(item);
            if (idx == -1) break;
            int space = MAX_STACK - cells[idx].quantity;
            int add   = std::min(space, remaining);
            cells[idx].quantity += add;
            remaining -= add;
        }
    }

    while (remaining > 0) {
        int idx = findFreeSlot();
        if (idx == -1) return false;
        int add = item->isStackable() ? std::min(MAX_STACK, remaining) : 1;

        Item* cloned = item->clone();
        int col = idx % COLS;
        int row = idx / COLS;
        cloned->set_pos(Point{
            drawX + gridOffsetX + col * CELL_SIZE,
            drawY + gridOffsetY + row * CELL_SIZE
        });

        cells[idx].item     = cloned;
        cells[idx].quantity = add;
        remaining -= add;
    }

    return true;
}


bool Bag::removeItem(int index, GameMap& gm){
    if (index < 0 || index >= (int)cells.size()) return false;
    if (cells[index].isEmpty())                  return false;

    delete cells[index].item;
    cells[index].item     = nullptr;
    cells[index].quantity = 0;
    return true;
}


void Bag::transferToInventory(int bagIdx, Inventory* inv, bool half) {
    if (!inv) return;
    if (bagIdx < 0 || bagIdx >= (int)cells.size()) return;
    BagCell& src = cells[bagIdx];
    if (src.isEmpty()) return;
    int toMove = half ? halfOf(src.quantity) : src.quantity;
    inv->add_item(src.item->clone(), toMove);
    src.quantity -= toMove;
    if (src.quantity <= 0) {
        delete src.item;
        src.item     = nullptr;
        src.quantity = 0;
    }
}


void Bag::transferFromInventory(int invIdx, Inventory* inv, bool half){
    if (!inv) return;
    InventorySlot* slot = inv->get_slot(invIdx);
    if (!slot || slot->isEmpty()) return;

    int toMove = half ? halfOf(slot->count) : slot->count;

    bool ok = addItem(slot->item, toMove);
    if (!ok) return;   // bag full

    slot->count -= toMove;
    if (slot->count <= 0) {
        inv->set_item_to_zero(invIdx);
    }
}


bool Bag::handleClick(float sx, float sy, bool rightButton, GameMap& gm){
    if (!open) return false;

    Inventory* inv = gm.player->get_inv();

    int bagIdx = cellAtPoint(sx, sy);
    if (bagIdx != -1) {
        transferToInventory(bagIdx, inv, rightButton);
        return true;
    }

    // Click on inventory slot
    // each slot is 43×43
    Point invPos = inv->get_pos();
    if (sx >= invPos.x && sy >= invPos.y &&
        sx < invPos.x + inv->get_max_item_count() * 43.f &&
        sy < invPos.y + inv->get_square_size())
    {
        int invIdx = (int)((sx - invPos.x) / 43.f);
        if (invIdx >= 0 && invIdx < inv->get_max_item_count()) {
            transferFromInventory(invIdx, inv, rightButton);
            return true;
        }
    }
    
    return false;
}

void Bag::drawStats(GameMap& gm){
    if (!font) return;

    float sx = drawX + w*0.55;
    float sy = drawY + h*0.2;

    al_draw_text(font, al_map_rgb(255, 255, 255),
                 sx, sy, 0, "Fatigue:");
    float barW  = 100.f;
    float barH  = 14.f;
    float barX  = sx + 70.f;
    float barY  = sy + 2.f;
    al_draw_filled_rectangle(barX, barY, barX + barW, barY + barH,
                             al_map_rgb(60, 60, 60));
    float fatiguePct = gm.player->get_fatigue_lvl() / max_fatigue;
    al_draw_filled_rectangle(barX, barY, barX + barW * fatiguePct, barY + barH,
                             al_map_rgb(220, 180, 50));
    al_draw_rectangle(barX, barY, barX + barW, barY + barH,
                      al_map_rgb(200, 200, 200), 1.f);

    int money = gm.player->get_money_amount();
    std::string moneyStr = "Gold: " + std::to_string(money);
    al_draw_text(font, al_map_rgb(255, 215, 0),
                 sx, sy + 26.f, 0, moneyStr.c_str());
}


void Bag::draw(GameMap& gm)
{
    if (!open) return;

    if (!gridImage) return;
    al_draw_bitmap(gridImage, drawX, drawY, 0);

    drawStats(gm);

    for (int i = 0; i < (int)cells.size(); ++i) {
        const BagCell& c = cells[i];
        if (c.isEmpty()) continue;

        int col = i % COLS;
        int row = i / COLS;
        float ix = gridOffsetX + col * CELL_SIZE;
        float iy = gridOffsetY + row * CELL_SIZE;
        float tx = drawX + ix + CELL_SIZE*0.1;
        float ty = drawY + iy + CELL_SIZE*0.1;

        c.item->draw({-1, -1}, CELL_SIZE, CELL_SIZE);

        if (c.item->isStackable() && c.quantity > 1 && font) {
            std::string qty = std::to_string(c.quantity);
            al_draw_text(font, al_map_rgb(255, 255, 255), tx, ty, 0, qty.c_str());
        }
    }
}


Item* Bag::getItem(int index) const
{
    if (index < 0 || index >= (int)cells.size()) return nullptr;
    return cells[index].item;
}

int Bag::getQuantity(int index) const
{
    if (index < 0 || index >= (int)cells.size()) return 0;
    return cells[index].quantity;
}
