#ifndef INCLUDE_CARROT
#define INCLUDE_CARROT
#include "Crop.hpp"


class Carrot final : public Crop {
public:
    static constexpr float GROW_TIME  = 180.f;
    static constexpr int   STAGES     = 4;

    Carrot(int tile_x, int tile_y);
    ~Carrot() override;

protected:
    void loadSprites()               override;
    void onStageChanged(int stage)   override;
    void onMature()                  override;
    void onDeath()                   override;
    float waterDrainMultiplier()     const override { return 0.6f; }
    std::vector<Item*> getDrops() const override;
};

#endif