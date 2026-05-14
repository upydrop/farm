#ifndef IncludeInventory
#define IncludeInventory
#include <vector>
#include "consts.hpp"
#include <allegro5/keyboard.h>
#include <allegro5/keycodes.h>
#include <allegro5/mouse.h>

class Item;
struct GameMap;

using namespace std;

struct InventorySlot {
    Item* item  = nullptr;
    int   count = 0;
    bool  isEmpty() const { return item == nullptr || count == 0; }
};

class Inventory { // singleton
    private:
        Point                   pos;
        ALLEGRO_FONT*           font = nullptr;
        vector<InventorySlot>   items;
        int                     active_item = -1;
        int                     item_count; // slot count
        int                     max_stack = 64;
        ALLEGRO_BITMAP*         inv_img = nullptr;
        ALLEGRO_BITMAP*         selected_img = nullptr;
        vector<ALLEGRO_BITMAP*> health_imgs;
        vector<ALLEGRO_BITMAP*> hunger_imgs;
        ALLEGRO_BITMAP*         health_bar_cache = nullptr;
        ALLEGRO_BITMAP*         hunger_bar_cache = nullptr;
        int                     cached_health = -1;
        int                     cached_hunger = -1;
        static const int        stat_size = 20;
        static const int        square_size = CELL_SIZE;
        static const int        height = square_size;
        static const int        item_max_count = 10;
        static const int        width = square_size * item_max_count;
        static Inventory*       instance;
        Inventory(Point pos_);
        void load_frames();
        void updateStatCache(ALLEGRO_BITMAP*& cache, int value, int maxValue, vector<ALLEGRO_BITMAP*>& imgs);
    public:
        Inventory(const Inventory& obj) = delete;
        void operator=(const Inventory& obj) = delete;
        static Inventory* get_instance(Point pos_ = {0.25*WIDTH, HEIGHT-square_size-10}){
            if (!instance) instance = new Inventory(pos_);
            return instance;
        };
        ~Inventory();

        int get_max_item_count() const { return item_max_count; }
        int get_item_count() const { return item_count; }
        int get_active_item_index() const { return active_item; }
        int get_square_size() const { return square_size; }
        Point get_pos() const { return pos; };

        void set_item_to_zero(int i);
        Item* take_item(int i);

        void item_update(GameMap& gmap);
        void draw_update(GameMap& gmap);
        bool handle_click(Point m_state);
        bool add_item(Item* item, int quantity = 1);

        bool isFull() const { return item_count >= item_max_count; }
        void remove_item(int i) {
            items.erase(items.begin() + i);
            active_item = -1;
        };
        Item* get_active_item() {
            if (active_item < 0 || active_item >= (int)items.size()) return nullptr;
            return items[active_item].item;
        };
        void set_active_item(int i) { if (!items[i].isEmpty()) active_item = i; }

        InventorySlot* get_slot(int i) {
            if (i < 0 || i >= (int)items.size()) return nullptr;
            return &items[i];
        }
        int get_slot_count(int i) const {
            if (i < 0 || i >= (int)items.size()) return 0;
            return items[i].count;
        }
};

#endif