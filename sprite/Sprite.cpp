#include "Sprite.hpp"
#include "../GameMap.hpp"


void Sprite::draw(GameMap &gmap, const Point* target_pos) {
    if (tilesetIndex >= 0) {
            const Tileset& ts = gmap.tilesets[tilesetIndex];
            int srcX = (tileID % ts.columns) * ts.tileW;
            int srcY = (tileID / ts.columns) * ts.tileH;
            float offsetY = -(target_pos->y - HEIGHT/2);
            float offsetX = -(target_pos->x - WIDTH/2);
            al_draw_bitmap_region(ts.texture,
                srcX, srcY, ts.tileW, ts.tileH,
                pos.x + offsetX, pos.y - ts.tileH + offsetY, 0);
    } else if (texture) {
        float offsetY = -(target_pos->y - HEIGHT/2);
        float offsetX = -(target_pos->x - WIDTH/2);
        al_draw_bitmap(texture, pos.x + offsetX, pos.y - get_h() + offsetY, 0);
    }
}