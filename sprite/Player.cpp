#include "Player.hpp"
#include "../storage/Inventory.hpp"
#include "../GameMap.hpp"
#include "../storage/Bag.hpp"
#include <cmath>

std::unordered_map<std::string, std::vector<ALLEGRO_BITMAP*>> load_frames() {
    std::unordered_map<std::string, std::vector<ALLEGRO_BITMAP*>> frames;
    std::vector<std::pair<std::string, std::string>> actions = {
        {"walk", "walking"},
        {"till", "tilling"},
    };

    std::vector<std::string> dirs = {"down", "up", "left", "right"};
    int frameCount = 6; // 0 до 5

    for (auto& [action, folder] : actions) {
        for (auto& dir : dirs) {
            std::string key = action + "_" + dir; // "walk_down", "till_up" ...
            
            for (int i = 0; i < frameCount; i++) {
                std::string path = "images/player/" + folder + "/" + dir + "/" 
                                   + std::to_string(i) + ".png";
                
                ALLEGRO_BITMAP* bmp = al_load_bitmap(path.c_str());
                
                if (bmp) {
                    frames[key].push_back(bmp);
                } else {
                    fprintf(stderr, "Failed to load: %s\n", path.c_str());
                }
            }
        }
    }
    return frames;
}

Player::Player(const std::string& name_, Point pos_, ALLEGRO_BITMAP* texture_, float w_, float h_):
    Sprite(name_, pos_, texture_, w_, h_) {
        inv = Inventory::get_instance();
        bag = new Bag();
        frames = load_frames();
    };

void Player::update(GameMap& gmap, const ALLEGRO_KEYBOARD_STATE& k_state, const ALLEGRO_MOUSE_STATE& m_state) {
    // cant exceed 90% fatigue
    if (v == player_speed_running && fatigue_lvl >= max_fatigue*0.9f){
        v = player_speed;
    }
    //player loses health when hungerPoints = 0. 0.49f to avoid float precisions as < 0.5f is 0 when rounded
    if (hungerPoints < 0.5f){
        healthPoints -= (healthPoints - dt/10 >= 0) ? dt/10 : healthPoints;
    } else if (hungerPoints > MAX_HUNGER-0.5f){
        healthPoints += (healthPoints + dt/3 <= MAX_HEALTH) ? dt/3 : MAX_HEALTH-healthPoints;
    }
    //death 
    if (healthPoints <= 0.f){
        cout << "death" << endl;
    }
    input(k_state);
    move(gmap);
    animate();
}

void Player::animate() {
    if (direction.x != 0)
        state = (direction.x > 0) ? "right" : "left";
    if (direction.y != 0)
        state = (direction.y > 0) ? "down" : "up";

    std::string animKey;
    if (currentState == CharacterState::Tilling){
        animKey = "till_" + state;
    }
    else{
        animKey = "walk_" + state;
    }
    auto& stateFrames = frames[animKey];

    bool isAction = (currentState != CharacterState::Idle && currentState != CharacterState::Walking);

    if (isAction) {
        frame_index += ACTION_SPEED_ANIM * dt;
        if (frame_index >= stateFrames.size()) {
            frame_index = 0.0f;
            actionDone = true;
            currentState = CharacterState::Idle;
        }
    } else {
        bool isMoving = (direction.x != 0 || direction.y != 0);
        if (!isMoving) frame_index = 0.0f;
        else frame_index += isMoving ? walking_speed_anim * dt : 0.0f;
    }

    int idx = static_cast<int>(frame_index) % stateFrames.size();
    set_texture(stateFrames[idx]);
}

void Player::set_current_state(CharacterState newState) {
    if (currentState != newState) {
        currentState = newState;
        frame_index = 0.0f;
        actionDone = false;
    }
}

void Player::input(const ALLEGRO_KEYBOARD_STATE& k_state) {
    direction.x = int(al_key_down(&k_state, ALLEGRO_KEY_D) || al_key_down(&k_state, ALLEGRO_KEY_RIGHT)) - int(al_key_down(&k_state, ALLEGRO_KEY_A) || al_key_down(&k_state, ALLEGRO_KEY_LEFT));
    direction.y = int(al_key_down(&k_state, ALLEGRO_KEY_S) || al_key_down(&k_state, ALLEGRO_KEY_DOWN)) - int(al_key_down(&k_state, ALLEGRO_KEY_W) || al_key_down(&k_state, ALLEGRO_KEY_UP));
    float len = sqrt(direction.x*direction.x + direction.y*direction.y);
    if (len != 0){
        direction.x /= len;
        direction.y /= len;
        if (v == player_speed_running){
            fatigue_lvl += (fatigue_lvl + 1/5 <= max_fatigue) ? 1/5 : max_fatigue - fatigue_lvl;
            hungerPoints -= (hungerPoints - dt/10 >= 0) ? dt/10 : 0;
        } else{
            fatigue_lvl -= (fatigue_lvl - 1/5 >= 0) ? 1/5 : fatigue_lvl;
            hungerPoints -= (hungerPoints - dt/20 >= 0) ? dt/20 : 0;
        }
    } else {
        hungerPoints -= (hungerPoints - dt/30 >= 0) ? dt/30 : 0;
        fatigue_lvl -= (fatigue_lvl - 1/5 >= 0) ? 1/5 : fatigue_lvl;
    }
}

void Player::check_collision(string type, GameMap& gmap){
    Point* player_pos = get_pos(); 
    float player_w = get_w();
    float player_h = get_h();

    for (int i = 0; i < gmap.collisions.size(); ++i){
        CollisionRect s = gmap.collisions[i];
        float rect_x = s.pos.x;
        float rect_y = s.pos.y;
        float rect_w = s.w;
        float rect_h = s.h;

        bool overlapX = (player_pos->x < rect_x + rect_w) && (player_pos->x + player_w > rect_x);
        bool overlapY = (player_pos->y < rect_y + rect_h) && (player_pos->y + player_h > rect_y);

        if (overlapX && overlapY) {
            if (type == "h"){
                float player_center_x = player_pos->x + player_w / 2.0f;
                float rect_center_x = rect_x + rect_w / 2.0f;

                if (player_center_x < rect_center_x) {
                    player_pos->x = rect_x - player_w - 1;
                } else {
                    player_pos->x = rect_x + rect_w + 1;
                }
                break;
            } else {
                float player_center_y = player_pos->y + player_h / 2.0f;
                float rect_center_y = rect_y + rect_h / 2.0f;

                if (player_center_y < rect_center_y) {
                    player_pos->y = rect_y - player_h - 1;
                } else {
                    player_pos->y = rect_y + rect_h + 1;
                }
                break;
            }
        }
    }

    for (const auto& s : gmap.collisionSprites){
        float sprite_x = s->get_pos()->x; 
        float sprite_y = s->get_pos()->y - s->get_h();
        float sprite_w = s->get_w();
        float sprite_h = s->get_h();

        bool overlapX = (player_pos->x < sprite_x + sprite_w) && (player_pos->x + player_w > sprite_x);
        bool overlapY = (player_pos->y < sprite_y + sprite_h) && (player_pos->y + player_h > sprite_y);

        if (!overlapX || !overlapY) continue;

        // depth
        float penetrationLeft   = (player_pos->x + player_w) - sprite_x;
        float penetrationRight  = (sprite_x + sprite_w) - player_pos->x;
        float penetrationTop    = (player_pos->y + player_h) - sprite_y;
        float penetrationBottom = (sprite_y + sprite_h) - player_pos->y;

        if (type == "h"){
            // Horizontal collision
            if (penetrationLeft < penetrationRight){
                player_pos->x = sprite_x - player_w - 1; 
            } else {
                player_pos->x = sprite_x + sprite_w + 1;
            }
            break; 
        } else { // vertical
            if (penetrationTop < penetrationBottom){
                player_pos->y = sprite_y - player_h - 1;
            } else {
                player_pos->y = sprite_y + sprite_h + 1; 
            }
            break;
        }
    }
}

void Player::move(GameMap& gmap) {
    Point *pos = get_pos();
    
    pos->y += direction.y * v;
    check_collision("v", gmap); 
    pos->x += direction.x * v;
    check_collision("h", gmap);
}

void Player::draw(GameMap& gmap, const Point* target_pos) {
    al_draw_bitmap(get_texture(), WIDTH/2, HEIGHT/2, 0);
}

void Player::change_hungerPoints(float points){
    if (hungerPoints + points <= MAX_HUNGER && hungerPoints + points >= 0){
        hungerPoints += points;
        return;
    } 
    hungerPoints = (hungerPoints + points < 0) ? 0 : MAX_HUNGER;
}

bool Player::change_moneyAmount(float money){
    if (money_amount + money >= 0){
        money_amount + money;
        return true;
    }
    return false;
}

void Player::change_healthPoints(float points){
    if (healthPoints + points >= 0 && healthPoints + points <= MAX_HEALTH){
        healthPoints += points;
        return;
    }
    healthPoints = (healthPoints + points < 0) ? 0 : MAX_HEALTH;
    return;
}