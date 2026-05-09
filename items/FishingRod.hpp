#ifndef INCLUDE_FISH_ROD
#define INCLUDE_FISH_ROD
#include "../items/Item.hpp"
#include "../consts.hpp"
#include <functional>
#include <cmath>
#include <cstdlib>

using namespace std;

enum class FishingState {
    Idle,
    Casting,
    Waiting,
    Reeling
};

struct Bobber {
    float x = 0, y = 0;
    float targetX = 0, targetY = 0;
    float speed = 400.f;
    bool inWater = false;
};

class FishingRod final : public Item {
    private:
        FishingState fishState = FishingState::Idle;
        Bobber bobber;

        ALLEGRO_BITMAP* bobberImg = nullptr;

        float rodTipX = 0.f, rodTipY = 0.f;

        float waitTimer    = 0.f;
        float waitDuration = 0.f;
        float reelTimer    = 0.f;
        float reelDuration = 1.2f;

        const float autoRetractRadius = 300.f;

        std::function<Item*(GameMap&)> lootFactory;

        void calcRodTip(GameMap& gmap);
        void spawnLoot(GameMap& gmap);

    public:
        FishingRod(Point pos_ = {0,0}, int healthPoints_ = 5, float use_cooldown_ = 0.3f, bool stackable_ = false): Item("Fishing Rod", "images/items/fishing_rod.png", pos_, healthPoints_, use_cooldown_, stackable_){
            bobberImg = al_load_bitmap("images/bobber.png");
            lootFactory = [](GameMap&) -> Item* {
                            static const int count = 4;
                            int r = rand() % count;
                            switch(r) {
                                case 0: return new WheatItem();
                                case 1: return new WheatSeed();
                                case 2: return new Hoe();
                                case 3: return new WateringCan();
                                default: return new WheatItem();
                            }
                        };
        }
        FishingRod(const FishingRod& other) : Item(other) {
            bobber = other.bobber;
            fishState = other.fishState;
            rodTipX = other.rodTipX;
            rodTipY = other.rodTipY;
            waitTimer = other.waitTimer;
            waitDuration = other.waitDuration;
            reelTimer = other.reelTimer;
            reelDuration = other.reelDuration;
            lootFactory = other.lootFactory;

            if (other.bobberImg) {
                bobberImg = al_load_bitmap("images/bobber.png");
            } else {
                bobberImg = nullptr;
            }
        }

        ~FishingRod() {
            if (bobberImg) al_destroy_bitmap(bobberImg);
        }

        void setLootFactory(std::function<Item*(GameMap&)> f) { lootFactory = f; }
        FishingState getFishingState() const { return fishState; }
        void retract();

        bool functionality(GameMap& gmap) override;

        void update(GameMap& gmap) override;
        void draw(Point pos_ = {-1,-1}, int new_w = -1, int new_h = -1, std::optional<std::reference_wrapper<GameMap>> gmap = std::nullopt) override;
        Item* clone() const override {
            return new FishingRod(*this);
        }
};

#endif