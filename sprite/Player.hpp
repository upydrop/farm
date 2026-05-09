#ifndef IncludePlayer
#define IncludePlayer
#include "Sprite.hpp"

class Inventory;
class Bag;

using namespace std;

enum class CharacterState {
    Idle,
    Walking,
    Tilling
};

class Player: public Sprite{
    private:
        CharacterState              currentState = CharacterState::Idle;
        Inventory*                  inv = nullptr;
        Bag*                        bag = nullptr;
        int                         v = player_speed;
        int                         walking_speed_anim = WALKING_SPEED_ANIM; 
        std::string                 state = "left";
        float                       frame_index = 0.0f;
        bool                        actionDone = false;
        Point                       direction = {0, 0};
        float                       fatigue_lvl = 0.f; //max_fatigue = 100.f;
        float                       money_amount = 10.f;
        float                       healthPoints = 4.f; //6 is max
        float                       hungerPoints = 1.f; //20 is max
        std::unordered_map<std::string, std::vector<ALLEGRO_BITMAP*>> frames; // frames[state_direction] = vector of file paths for animation frames of that state
    public:
        Inventory*  get_inv()       const { return inv; }
        Bag*        getBag()        const { return bag; }
        string      get_state()     const { return state; }
        Point       get_direction() const { return direction; }

        Player(const std::string& name_, Point pos_, ALLEGRO_BITMAP* texture_, float w_, float h_);

        float get_fatigue_lvl() const { return fatigue_lvl; }
        float get_healthPoints() const { return healthPoints; }
        float get_hungerPoints() const { return hungerPoints; }
        float get_money_amount() const { return money_amount; }

        void set_current_state(CharacterState newState);
        void set_walking_speed_anim(int new_speed) { walking_speed_anim = new_speed; }
        void set_v(int new_v) { v = new_v; }

        void change_hungerPoints(float points);
        void change_healthPoints(float points);
        bool change_moneyAmount(float money); 

        void update(GameMap &gmap, const ALLEGRO_KEYBOARD_STATE& k_state = {}, const ALLEGRO_MOUSE_STATE& m_state = {}) override;
        void input(const ALLEGRO_KEYBOARD_STATE& k_state);
        void animate();
        void move(GameMap &gmap);
        void check_collision(string type, GameMap& gmap);
        void draw(GameMap& gmap, const Point* target_pos) override;
};

#endif