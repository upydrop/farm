#include "eventHandler.hpp"
#include "../GameMap.hpp"
#include "../storage/Inventory.hpp"
#include "../items/Item.hpp"
#include "../crops/Wheat.hpp"
#include "../storage/Bag.hpp"
#include <cmath>
#include <algorithm>

void handle_event(GameMap& gmap, const ALLEGRO_MOUSE_STATE m_state, const ALLEGRO_KEYBOARD_STATE& k_state, float camX, float camY){
    static bool q_was_down = false;
    static bool t_was_down = false;
    static bool lmb_was_down = false;
    static bool rmb_was_down = false;

    auto onScreen = [&](float x, float y, float w, float h) {
                    return x + w >= camX-50 && x <= camX + WIDTH+50 &&
                        y >= camY-50 && y - h <= camY + HEIGHT+50;
                };

    bool lmb_is_down = al_mouse_button_down(&m_state, ALLEGRO_MOUSE_BUTTON_LEFT);
    bool rmb_is_down = al_mouse_button_down(&m_state, ALLEGRO_MOUSE_BUTTON_RIGHT);
    bool q_is_down = al_key_down(&k_state, ALLEGRO_KEY_Q);
    bool t_is_down = al_key_down(&k_state, ALLEGRO_KEY_T);

    if (q_is_down && !q_was_down){
        Inventory* inv = gmap.player->get_inv();
        int idx = inv->get_active_item_index();

        Item* item = inv->take_item(idx);
        if (item) {
            item->set_pos(Point{
                gmap.player->get_pos()->x,
                gmap.player->get_pos()->y
            });
            gmap.dropped_loot.push_back(item);
        }
    }
    q_was_down = q_is_down;
    
    if (al_key_down(&k_state, ALLEGRO_KEY_E)){
        if (gmap.player->get_inv()->get_item_count() < gmap.player->get_inv()->get_max_item_count()){
            for (int i = 0; i < gmap.dropped_loot.size(); ++i){
                Item* loot = gmap.dropped_loot[i];
                Point L_pos = loot->get_pos();
                if (onScreen(L_pos.x, L_pos.y, loot->get_w(), loot->get_h())){
                    Point* P_pos = gmap.player->get_pos();
                    if ((sqrt(pow((L_pos.x + loot->get_w()/2) - (P_pos->x + gmap.player->get_w()/2), 2) + pow((L_pos.y + loot->get_h()/2) - (P_pos->y + gmap.player->get_h()/2), 2)) <= LOOT_PICK_UP_RADIUS)){
                        if (gmap.player->get_inv()->add_item(loot->clone())){
                            gmap.dropped_loot.erase(gmap.dropped_loot.begin() + i);
                        }
                        return;
                    }
                }
            }
        }
    }

    if (al_key_down(&k_state, ALLEGRO_KEY_LSHIFT) || al_key_down(&k_state, ALLEGRO_KEY_RSHIFT)) {
        gmap.player->set_v(player_speed_running);
        gmap.player->set_walking_speed_anim(RUNNING_SPEED_ANIM);
    } else {
        gmap.player->set_v(player_speed);
        gmap.player->set_walking_speed_anim(WALKING_SPEED_ANIM);
    }

    if (t_is_down && !t_was_down){
        gmap.player->getBag()->changeOpen();
    }
    t_was_down = t_is_down;

    gmap.player->update(gmap, k_state);
    

    if (lmb_is_down){
        Bag *bag = gmap.player->getBag();
        if (bag->isOpen() && !lmb_was_down){
            bag->handleClick(m_state.x, m_state.y, false, gmap);//false = not rmb
        }
        if (!gmap.player->get_inv()->handle_click(Point{(float)m_state.x, (float)m_state.y})){
            int act_item_idx = gmap.player->get_inv()->get_active_item_index();
            if (act_item_idx >= 0){
                Item* activeItem = gmap.player->get_inv()->get_active_item();
                if (activeItem != nullptr){
                    float now = al_get_time();
                    if (now - activeItem->get_last_used() >= activeItem->get_cooldown()){
                        activeItem->set_last_used(now);
                        gmap.mouseWorldPos = {(float)m_state.x + camX, (float)m_state.y + camY};
                        if (activeItem->functionality(gmap)){ // item broke
                            gmap.player->get_inv()->set_item_to_zero(act_item_idx);
                            return;
                        }
                    }
                }
            }
            float worldX = m_state.x + camX;
            float worldY = m_state.y + camY;

            harvestCropAtPosition(gmap, worldX, worldY);
        }
    }
    lmb_was_down = lmb_is_down;

    if (rmb_is_down){
        Bag *bag = gmap.player->getBag();
        if (bag->isOpen() && !rmb_was_down){
            bag->handleClick(m_state.x, m_state.y, true, gmap); //true = rmb
        }
    }
    rmb_was_down = rmb_is_down;
}

