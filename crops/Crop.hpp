#ifndef INCLUDE_CROP
#define INCLUDE_CROP
#include "../consts.hpp"
#include "../GameMap.hpp"

using namespace std;

enum class CropState {
    SEED,      
    GROWING,   
    MATURE,    
    DEAD     
};

class Crop {
    private:
        void advanceStage();
        void drawSprite(int stage) const;

        int         tile_x, tile_y;

        int         num_stages; 
        int         current_stage;     
        float       time_per_stage;
        float       stage_timer; 
        bool        on_watered_tile;

        float       water_level; 
        float       wilt_timer;

        CropState   state;
    protected:
        virtual void loadSprites() {}
        virtual void onStageChanged(int new_stage) {}

        virtual void onMature() {}

        virtual void onDeath() {}

        // Возвращает коэффициент: 1.0 = норма, 2.0 = пьёт в два раза быстрее.
        virtual float waterDrainMultiplier() const { return 1.f; }

        std::vector<ALLEGRO_BITMAP*> sprites;
    public:
        static constexpr float WILT_TIME          = 120.f; // время, в течение которого растение может оставаться без воды, прежде чем погибнуть
        static constexpr float WATER_DRAIN_RATE   = 0.5f;

        Crop(int tile_x, int tile_y,
            float total_grow_time, 
            int   num_stages); 

        virtual ~Crop();

        bool update(float delta, GameMap& gmap);

        void draw() const;

        void water(float water_amount = 30.f);

        virtual std::vector<Item*> getDrops() const = 0;

        bool isOnWateredTile() const { return on_watered_tile; }
        CropState getState()        const { return state; }
        bool      isDead()          const { return state == CropState::DEAD; }
        bool      isMature()        const { return state == CropState::MATURE; }
        int       getCurrentStage() const { return current_stage; }
        float     getGrowProgress() const;
        float     getWaterLevel()   const { return water_level; }
        int       getTileX()        const { return tile_x; }
        int       getTileY()        const { return tile_y; }
        float     getSpriteH()      const { return sprites[current_stage] ? al_get_bitmap_height(sprites[current_stage]) : TILESIZE; }
        float     getSpriteW()      const { return sprites[current_stage] ? al_get_bitmap_width(sprites[current_stage]) : TILESIZE; }

        void setOnWateredTile(bool watered) { on_watered_tile = watered; }
};

#endif