#include <windows.h>
#include "funcs/eventHandler.hpp"
#include "GameMap.hpp"
#include "storage/Inventory.hpp"
#include "items/Item.hpp"
#include "storage/Bag.hpp"
#include <chrono>
#include <algorithm>
#include "crops/Crop.hpp"
#include <stdexcept>

class Screen : public Engine {
    private:
        ALLEGRO_MOUSE_STATE m_state;
        ALLEGRO_KEYBOARD_STATE k_state;
        GameMap gmap;
        vector<Sprite*> allSprites;
        bool spritesDirty = true;
        float camX = 0, camY = 0;
        ALLEGRO_FONT* font = nullptr;
    public:
        Screen(){
            loadMap(gmap, "data/maps/map.tmx");
            gmap.dropped_loot.push_back({new Hoe(Point{200, 200}, 10), 1});
            gmap.worktables.push_back(std::make_unique<Workbench>(300, 300));
            font = al_create_builtin_font();
            if (!font) throw "Failed to create font in screen init";
        };
        void physics_process() override {
            al_get_mouse_state(&m_state);
            al_get_keyboard_state(&k_state);
            handle_event(gmap, m_state, k_state, camX, camY);
            camX = gmap.player->get_pos()->x - WIDTH/2;
            camY = gmap.player->get_pos()->y - HEIGHT/2;

            float local_dt = dt;
            GameMap& gmap_ref = gmap;
            gmap.crops.erase(
            std::remove_if(gmap.crops.begin(), gmap.crops.end(),
                [local_dt, this](const std::unique_ptr<Crop>& c) {
                    return !c->update(local_dt, gmap);
                }),
                gmap.crops.end());

            gmap.player->get_inv()->item_update(gmap);

            for (const auto& wb : gmap.worktables) {
                wb->update(gmap);
            }
        }
        void render_process() override {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            if (gmap.mapBuffer)
                al_draw_bitmap_region(gmap.mapBuffer,
                    camX, camY, WIDTH, HEIGHT,
                    0, 0, 0);

                auto onScreen = [&](float x, float y, float w, float h) {
                    return x + w >= camX-50 && x <= camX + WIDTH+50 &&
                        y >= camY-50 && y - h <= camY + HEIGHT+50;
                };

                auto onScreenC = [&](float x, float y, float w, float h) {
                    return x + w >= camX && x <= camX + WIDTH &&
                        y + h >= camY && y <= camY + HEIGHT;
                };

                float offsetY = -camY;
                float offsetX = -camX;

                for (const auto& item : gmap.dropped_loot){
                    Point pos = item.item->get_pos();
                    if (!onScreenC(pos.x, pos.y, 30, 30)) continue;
                    item.item->draw(Point{pos.x + offsetX, pos.y + offsetY}, 30, 30);
                };

                ALLEGRO_TRANSFORM t, old_t;
                al_copy_transform(&old_t, al_get_current_transform());
                al_identity_transform(&t);
                al_translate_transform(&t, -(gmap.player->get_pos()->x - WIDTH/2),
                                        -(gmap.player->get_pos()->y - HEIGHT/2));
                al_use_transform(&t);
                for (const auto& crop : gmap.crops) {
                    float wx = crop->getTileX() * TILESIZE;
                    float wy = crop->getTileY() * TILESIZE;
                    if (!onScreen(wx, wy, TILESIZE, crop->getSpriteH())) continue;

                    crop->draw();

                }
                al_use_transform(&old_t);

                if (spritesDirty) {
                    allSprites = gmap.sprites;
                    allSprites.push_back(gmap.player);
                    allSprites.insert(allSprites.end(),
                        gmap.collisionSprites.begin(),
                        gmap.collisionSprites.end());
                    spritesDirty = false;
                }
                std::sort(allSprites.begin(), allSprites.end(), [](Sprite* a, Sprite* b) {
                    return a->get_pos()->y + a->get_h()/2 < b->get_pos()->y + b->get_h()/2;
                });

                for (const auto& s : allSprites) {
                    if (!onScreen(s->get_pos()->x, s->get_pos()->y - (s->get_tilesetIndex() >= 0 ?
                        gmap.tilesets[s->get_tilesetIndex()].tileH : 0), s->get_w(), s->get_h())) continue;
                    s->draw(gmap, gmap.player->get_pos());
                };

                for (const auto& wb : gmap.worktables) {
                    if (!onScreen(wb->worldX, wb->worldY, 50, 50)) continue;
                    wb->drawSprite(camX, camY);
                    if (wb->isOpen()) {
                        wb->draw(gmap);
                    }
                }

                gmap.player->get_inv()->draw_update(gmap);

                gmap.player->getBag()->draw(gmap);

                if (gmap.held_item){
                    gmap.held_item->draw(Point{(float)m_state.x, (float)m_state.y}, 30, 30);
                    al_draw_textf(font, al_map_rgb(255, 255, 255),
                        m_state.x+30, m_state.y, ALLEGRO_ALIGN_RIGHT, "%d", gmap.held_item_quantity);
                }
        }
};

int main()
{
    try {
        if (!al_init()) throw std::runtime_error("Failed to init Allegro");
        al_init_primitives_addon();
        al_init_image_addon();
        al_install_mouse();
        al_install_keyboard();
        al_init_font_addon();
        al_init_ttf_addon();

        std::cout << "Initializing screen" << std::endl;
        Screen screen;
        
        std::cout << "Starting engine" << std::endl;
        screen.start();

    } catch (const std::exception& e) {
        std::cerr << "Standard Exception: " << e.what() << std::endl;
    } catch (const char* error) {
        std::cerr << "String Exception: " << error << std::endl;
    } catch (...) {
        std::cerr << "Unknown error" << std::endl;
    }

    return 0;
}


