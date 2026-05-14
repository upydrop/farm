#ifndef IncludeItem
#define IncludeItem
#include "../sprite/Sprite.hpp"
#include "../GameMap.hpp"
#include <cmath>
#include <optional>

using namespace std;

class Item{
    private:
        string          name;
        string          img_path;
        Point           pos;
        ALLEGRO_BITMAP *img = nullptr;
        bool            stackable;
        int             healthPoints;
        float           use_cooldown;
        float           last_used = -999.0f;
        int             w;
        int             h;
    public:
        Item(string name_, string img_path_, Point pos = Point{-1, -1}, int healthPoints = 0, float use_cooldown = 0.2f, bool stackable = false);
        Item(const Item& other); //for copying
        ~Item();

        int get_w() const { return w; }
        int get_h() const { return h; }
        string get_name() const { return name; }
        float get_last_used() const { return last_used; }
        float get_cooldown() const { return use_cooldown; }
        int get_healthPoints() { return healthPoints; }
        Point get_pos() const { return pos; }

        bool isStackable() const { return stackable; }
        void change_healthPoints(int amount) { healthPoints += amount; }
        void set_last_used(float time) { last_used = time; }
        void set_pos(Point pos_) { pos = pos_; }
        
        virtual void update(GameMap& gmap) {}
        virtual void draw(Point pos_ = {-1, -1} ,int new_w = -1, int new_h = -1, std::optional<std::reference_wrapper<GameMap>> gmap = std::nullopt);
        virtual bool functionality(GameMap& gmap) = 0; //return true = item broke
        virtual Item* clone() const = 0;
};

class Hoe final: public Item {
    public:
        Hoe(Point pos_ = {-1, -1}, int healthPoints_ = 10, float use_cooldown_ = 0.07f, bool stackable_ = false) : Item("Hoe", "images/items/hoe.png", pos_, healthPoints_, use_cooldown_, stackable_){};
        bool functionality(GameMap& gmap) override;
        Hoe(const Hoe& other) : Item(other) {}
        Item* clone() const override {
            return new Hoe(*this);
        }
};

class WateringCan final: public Item {
    private:
        float water_level = 100.f; // от 0 до 100
        float water_usage = 10.f;
    public:
        float get_water_level() const { return water_level; }
        WateringCan(Point pos_ = {-1, -1}, int healthPoints_ = 0, float use_cooldown_ = 0.2f, bool stackable_ = false) : Item("Watering Can", "images/items/watering_can.png", pos_, healthPoints_, use_cooldown_, stackable_) {};
        bool functionality(GameMap& gmap) override;
        Item* clone() const override {
            return new WateringCan(*this);
        }
};

class Bread final: public Item {
    public:
        Bread(Point pos_ = {-1, -1}, int healthPoints_ = 0, float use_cooldown_ = 0.2f, bool stackable_ = true) : Item("Bread", "images/items/bread.png", pos_, healthPoints_, use_cooldown_, stackable_) {};
        bool functionality(GameMap& gmap) override;
        Item* clone() const override {
            return new Bread(*this);
        }
};

class WheatSeed final: public Item {
    public:
        WheatSeed(Point pos_ = {-1, -1}, int healthPoints_ = 0, float use_cooldown_ = 0.2f, bool stackable_ = true) : Item("Wheat Seed", "images/items/wheat_seed.png", pos_, healthPoints_, use_cooldown_, stackable_){};
        bool functionality(GameMap& gmap) override;
        Item* clone() const override {
            return new WheatSeed(*this);
        }
};

class WheatItem final: public Item {
    public:
        WheatItem(Point pos_ = {-1, -1}, int healthPoints_ = 0, float use_cooldown_ = 0.2f, bool stackable_ = true) : Item("Wheat", "images/items/wheat.png", pos_, healthPoints_, use_cooldown_, stackable_) {};
        bool functionality(GameMap& gmap) override { return false; }
        Item* clone() const override {
            return new WheatItem(*this);
        }
};

class CarrotSeed final: public Item {
    public:
        CarrotSeed(Point pos_ = {-1, -1}, int healthPoints_ = 0, float use_cooldown_ = 0.2f, bool stackable_ = true) : Item("Carrot Seed", "images/items/carrot_seed.png", pos_, healthPoints_, use_cooldown_, stackable_){};
        bool functionality(GameMap& gmap) override;
        Item* clone() const override {
            return new CarrotSeed(*this);
        }
};

class CarrotItem final: public Item {
    public:
        CarrotItem(Point pos_ = {-1, -1}, int healthPoints_ = 0, float use_cooldown_ = 0.2f, bool stackable_ = true) : Item("Carrot", "images/items/carrot.png", pos_, healthPoints_, use_cooldown_, stackable_) {};
        bool functionality(GameMap& gmap) override;
        Item* clone() const override {
            return new CarrotItem(*this);
        }
};

class CarrotCake final: public Item {
    public:
        CarrotCake(Point pos_ = {-1, -1}, int healthPoints_ = 0, float use_cooldown_ = 0.2f, bool stackable_ = true) : Item("CarrotCake", "images/items/carrot_cake.png", pos_, healthPoints_, use_cooldown_, stackable_) {};
        bool functionality(GameMap& gmap) override;
        Item* clone() const override {
            return new CarrotCake(*this);
        }
};

#endif