#include "Crop.hpp"
#include "../funcs/mapFuncs.hpp"
#include <algorithm>
#include <cassert>

Crop::Crop(int tile_x_, int tile_y_,
           float total_grow_time,
           int   num_stages_)
    : tile_x(tile_x_)
    , tile_y(tile_y_)
    , num_stages(num_stages_)
    , current_stage(0)
    , stage_timer(0.f)
    , water_level(50.f) // старт с 50% воды
    , wilt_timer(0.f)   
    , state(CropState::SEED)
{
    assert(num_stages >= 2 && "Нужно минимум 2 стадии");

    time_per_stage = total_grow_time / static_cast<float>(num_stages - 1);

    sprites.assign(num_stages, nullptr);
}

Crop::~Crop() {
    for (auto& bmp : sprites) {
        bmp = nullptr;
    }
}

bool Crop::update(float delta, GameMap& gmap) {
    if (state == CropState::DEAD) return false;

    float drain = WATER_DRAIN_RATE * waterDrainMultiplier() * delta;
    water_level = std::max(0.f, water_level - drain);

    if (water_level <= 0.f) {
        wilt_timer += delta;
        if (on_watered_tile) {
            setTile(gmap, 0, tile_y * gmap.mapW + tile_x    , tileTypes["plowed"].tileID, tileTypes["plowed"].tilesetIndex);
            on_watered_tile = false;
            update_buffer(gmap);
        }
        if (wilt_timer >= WILT_TIME) {
            state = CropState::DEAD;
            onDeath();
            return false;
        }
    } else {
        wilt_timer = std::max(0.f, wilt_timer - delta * 2.f); // восстанавливаем в 2× быстрее
    }

    if (state == CropState::MATURE) return true;

    stage_timer += delta;

    if (stage_timer >= time_per_stage) {
        stage_timer -= time_per_stage;
        advanceStage();
    }

    return true;
}
void Crop::advanceStage() {
    if (current_stage >= num_stages - 1) return;

    ++current_stage;
    onStageChanged(current_stage);

    if (current_stage == num_stages - 1) {
        state = CropState::MATURE;
        onMature();
    } else if (current_stage == 0) {
        state = CropState::SEED;
    } else {
        state = CropState::GROWING;
    }
}
void Crop::draw() const {
    int stage = current_stage;

    if (stage < static_cast<int>(sprites.size()) && sprites[stage] != nullptr) {
        drawSprite(stage);
    }

    if (water_level < 30.f || wilt_timer > 0.f) {
        float sx = static_cast<float>(tile_x * TILESIZE);
        float sy = static_cast<float>(tile_y * TILESIZE);
        float bar_w  = static_cast<float>(TILESIZE - 4);
        float bar_h  = 4.f;
        float fill_w = bar_w * (water_level / 100.f);
        float bx = sx + 2.f;
        float by = sy + TILESIZE*0.95f - bar_h;

        al_draw_filled_rectangle(bx, by, bx + bar_w, by + bar_h,
                                 al_map_rgba(0, 0, 0, 160));
        ALLEGRO_COLOR water_col = (water_level < 15.f)
            ? al_map_rgb(220, 60, 60)
            : al_map_rgb(60, 140, 220);
        if (fill_w > 0.f)
            al_draw_filled_rectangle(bx, by, bx + fill_w, by + bar_h, water_col);
    }
}
void Crop::drawSprite(int stage) const {
    ALLEGRO_BITMAP* bmp = sprites[stage];
    int bw = al_get_bitmap_width(bmp);
    int bh = al_get_bitmap_height(bmp);

    float tile_px = static_cast<float>(tile_x * TILESIZE);
    float tile_py = static_cast<float>(tile_y * TILESIZE);

    float draw_x = tile_px + (TILESIZE - bw) * 0.5f;
    float draw_y = tile_py + (TILESIZE - bh); 

    al_draw_bitmap(bmp, draw_x, draw_y, 0);
}

void Crop::water(float water_amount) {
    if (state == CropState::DEAD) return;
    water_level = std::min(100.f, water_level + water_amount);
}

float Crop::getGrowProgress() const {
    if (state == CropState::MATURE) return 1.f;
    if (state == CropState::DEAD)   return 0.f;

    float stage_progress = stage_timer / time_per_stage;
    float total_progress = (static_cast<float>(current_stage) + stage_progress)
                           / static_cast<float>(num_stages - 1);
    return std::clamp(total_progress, 0.f, 1.f);
}