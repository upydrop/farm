#ifndef BAG_INCLUDE
#define BAG_INCLUDE

#include <vector>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include "../items/Item.hpp"
#include "../sprite/Player.hpp"
#include "Inventory.hpp"
#include "../GameMap.hpp"

class Bag {
public:
    static const int COLS        = 10;
    static const int ROWS        = 5;
    static const int CELL_SIZE   = 43;
    static const int MAX_STACK   = 64;

private:
    struct BagCell {
        Item* item     = nullptr;
        int   quantity = 0;
        bool  isEmpty() const { return item == nullptr || quantity == 0; }
    };

    std::vector<BagCell> cells;

    ALLEGRO_BITMAP* gridImage = nullptr;
    ALLEGRO_FONT*   font      = nullptr;
    float w;
    float h;
    float gridOffsetX = 32.f;  //grid start at x
    float gridOffsetY = 142.f;  //grid start at y

    bool open = false;

    float drawX = 0;
    float drawY = 0;

    int  findStackSlot(Item* item) const;   // index or -1
    int  findFreeSlot()            const;   // index or -1
    bool canStack(Item* a, Item* b) const;

    int  cellAtPoint(float sx, float sy) const;

    int  halfOf(int count) const;

public:
    Bag(ALLEGRO_BITMAP* img = al_load_bitmap("images/bag/bag.png"), ALLEGRO_FONT* fnt = al_create_builtin_font());
    ~Bag();

    bool addItem(Item* item, int quantity = 1);

    bool removeItem(int index, GameMap& gm);

    void transferToInventory (int bagIdx,  Inventory* inv, bool half);
    void transferFromInventory(int invIdx, Inventory* inv, bool half);

    // called from event_handler
    bool handleClick(float sx, float sy, bool rightButton, GameMap& gm);

    void draw(GameMap& gm);

    bool isOpen()          const { return open; }
    void setOpen(bool val)       { open = val;  }
    void changeOpen()            { open = (open) ? false : true; }
    void toggle()                { open = !open; }

    Item* getItem    (int index) const;
    int   getQuantity(int index) const;

    void drawStats(GameMap& gm);
};

#endif
