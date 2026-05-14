#include "Inventory.hpp"
#include "../items/Item.hpp"
#include "../items/FishingRod.hpp"
#include "../sprite/Player.hpp"
#include <algorithm>

static const int MAX_HEALTH_INT = 6;
static const int MAX_HUNGER_INT = 20;

Inventory* Inventory::instance = nullptr;

void Inventory::load_frames() {
    // 0 - full, 1 - half, 2 - empty
    health_imgs.push_back(al_load_bitmap("images/stats/hearts/full.png"));
    health_imgs.push_back(al_load_bitmap("images/stats/hearts/half.png"));
    health_imgs.push_back(al_load_bitmap("images/stats/hearts/empty.png"));
    
    //reversed
    hunger_imgs.push_back(al_load_bitmap("images/stats/food/empty.png"));
    hunger_imgs.push_back(al_load_bitmap("images/stats/food/half.png"));
    hunger_imgs.push_back(al_load_bitmap("images/stats/food/full.png"));
}

Inventory::Inventory(Point pos_){
    pos = pos_;
    item_count = 0;
    inv_img = al_load_bitmap("images/storage/inventory/inv.png");
    if (!inv_img) throw "Failed to load inventory image";
    selected_img = al_load_bitmap("images/storage/inventory/selected.png");
    if (!selected_img) throw "Failed to load selected image";
    load_frames();
    font = al_create_builtin_font();
    if (!font) throw "Failed to create font";
    items.push_back(InventorySlot{new Hoe(), 1}); //tests
    items.push_back(InventorySlot{new WheatItem(), 6}); //tests
    items.push_back(InventorySlot{new WheatSeed(), 1}); //tests
    items.push_back(InventorySlot{new WheatSeed(), 1}); //tests
    items.push_back(InventorySlot{new WateringCan(), 1}); //tests
    items.push_back(InventorySlot{new FishingRod(), 1}); //tests
    items.push_back(InventorySlot{new Bread(), 30}); //tests
    items.push_back(InventorySlot{new CarrotItem(), 30}); //tests
    items.push_back(InventorySlot{new CarrotSeed(), 30}); //tests
    items.push_back(InventorySlot{new CarrotCake(), 1}); //tests
    item_count+=10; //tests
}

void Inventory::item_update(GameMap& gmap){
    for (const auto& slot : items){
        if (slot.item != nullptr){
            slot.item->update(gmap);
        }
    };
}

void Inventory::updateStatCache(ALLEGRO_BITMAP*& cache, int stat, int maxValue, vector<ALLEGRO_BITMAP*>& imgs) {
    stat = std::max(0, std::min(stat, maxValue));

    int numIcons = maxValue / 2;
    int barWidth = stat_size * numIcons;

    if (cache) {
        al_destroy_bitmap(cache);
        cache = nullptr;
    }
    cache = al_create_bitmap(barWidth, stat_size);

    ALLEGRO_BITMAP* prev_target = al_get_target_bitmap();
    al_set_target_bitmap(cache);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));

    for (int i = 1; i <= numIcons; ++i) {
        int lo = (i - 1) * 2 + 1;
        int hi = i * 2;

        int imgIndex;
        if (stat >= hi)      imgIndex = 0;
        else if (stat >= lo) imgIndex = 1;
        else                 imgIndex = 2;

        al_draw_bitmap(imgs[imgIndex], stat_size * (i - 1), 0, 0);
    }

    al_set_target_bitmap(prev_target);
}

void Inventory::draw_update(GameMap& gmap) {
    if (inv_img) al_draw_bitmap(inv_img, pos.x, pos.y, 0);
    if (active_item >= 0 && selected_img) {
        al_draw_bitmap(selected_img, pos.x + active_item * square_size, pos.y, 0);
    }

    for (int i = 0; i < items.size(); ++i) {
        if (items[i].item != nullptr) {
            items[i].item->draw(Point{pos.x + square_size * i, pos.y}, -1, -1, gmap);
            if (items[i].count > 1) {
                al_draw_textf(font, al_map_rgb(255, 255, 255),
                    pos.x + square_size * i + square_size * 0.1,
                    pos.y + square_size * 0.1,
                    ALLEGRO_ALIGN_LEFT, "%d", items[i].count);
            }
        }
    }

    int pHealth = (int)round(gmap.player->get_healthPoints());
    int pHunger = (int)round(gmap.player->get_hungerPoints());

    if (!health_bar_cache || pHealth != cached_health) {
        cached_health = pHealth;
        updateStatCache(health_bar_cache, pHealth, MAX_HEALTH_INT, health_imgs);
}
    if (!hunger_bar_cache) {
        cached_hunger = pHunger;
        updateStatCache(hunger_bar_cache, MAX_HUNGER_INT - pHunger, MAX_HUNGER_INT, hunger_imgs);
    }

    if (!hunger_bar_cache || pHunger != cached_hunger) {
        cached_hunger = pHunger;
        updateStatCache(hunger_bar_cache, MAX_HUNGER_INT - pHunger, MAX_HUNGER_INT, hunger_imgs);
    }

    al_draw_bitmap(health_bar_cache, pos.x, pos.y - square_size / 2, 0);
    al_draw_bitmap(hunger_bar_cache, pos.x + width - al_get_bitmap_width(hunger_bar_cache), pos.y - square_size / 2, 0);
}

bool Inventory::handle_click(Point m_state){
    if ((pos.x <= m_state.x && m_state.x <= pos.x+width) && (pos.y <= m_state.y && m_state.y <= pos.y+height)){
        active_item = floor((m_state.x-pos.x)/square_size);
        return true;
    }
    return false;
}

bool Inventory::add_item(Item* item, int quantity) {
    int remaining = quantity;

    if (item->isStackable()) {
        for (int i = 0; i < (int)items.size() && remaining > 0; ++i) {
            if (!items[i].isEmpty() && items[i].item->get_name() == item->get_name()
                && items[i].count < max_stack)
            {
                int space = max_stack - items[i].count;
                int add   = std::min(space, remaining);
                items[i].count += add;
                remaining -= add;
            }
        }
    }

    while (remaining > 0 && item_count < item_max_count) {
        int add = item->isStackable() ? std::min(max_stack, remaining) : 1;

        bool placed = false;
        for (int i = 0; i < (int)items.size(); ++i) {
            if (items[i].isEmpty()) {
                items[i] = InventorySlot{item->clone(), add};
                item_count++;
                placed = true;
                break;
            }
        }
        if (!placed && (int)items.size() < item_max_count) {
            items.push_back(InventorySlot{item->clone(), add});
            item_count++;
        } else if (!placed) {
            return false; // инвентарь полон
        }

        remaining -= add;
    }
    delete item; // оригинал больше не нужен, везде использовались clone()
    return true;
}

void Inventory::set_item_to_zero(int i){
    if (i < 0 || i >= (int)items.size() || items[i].item == nullptr) return;
    if (items[i].item->isStackable()){
        items[i].count--;
        if (items[i].count <= 0){
            delete items[i].item;
            items[i].item = nullptr;
            item_count--;
        }
    } else {
        delete items[i].item;
        items[i].item = nullptr;
        item_count--;
    }
}

Item* Inventory::take_item(int i) {
    if (i < 0 || i >= (int)items.size()) return nullptr;

    Item* item = items[i].item;
    if (!item) return nullptr;

    if (item->isStackable()) {

        items[i].count--;

        if (items[i].count <= 0) {
            items[i].item = nullptr;
            item_count--;
            return item;
        }

        return item->clone();
    } else {
        items[i].item = nullptr;
        item_count--;
        return item;
    }
}

Inventory::~Inventory(){
    if (inv_img) al_destroy_bitmap(inv_img);
    if (selected_img) al_destroy_bitmap(selected_img);
    if (font) al_destroy_font(font);
    for (auto& slot : items) {
        if (slot.item) {
            delete slot.item;
            slot.item = nullptr;
        }
    }
}