#ifndef INCLUDE_WHEAT
#define INCLUDE_WHEAT
#include "Crop.hpp"


class Wheat final : public Crop {
public:
    static constexpr float GROW_TIME  = 120.f;
    static constexpr int   STAGES     = 4;

    Wheat(int tile_x, int tile_y);
    ~Wheat() override;

protected:
    void loadSprites()               override;
    void onStageChanged(int stage)   override;
    void onMature()                  override;
    void onDeath()                   override;
    float waterDrainMultiplier()     const override { return 0.8f; }
    std::vector<Item*> getDrops() const override;
};

#endif