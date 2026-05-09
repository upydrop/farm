#ifndef IncludeSprite
#define IncludeSprite
#include "../consts.hpp"

struct GameMap;

class Sprite{
    private:
        std::string name;
        Point pos;
        float w, h;
        int tilesetIndex;
        int tileID;
        ALLEGRO_BITMAP* texture = nullptr;
    public:
        Sprite(const std::string& name_, Point pos_, ALLEGRO_BITMAP* texture_ = nullptr, float w_ = 0, float h_ = 0, int tilesetIndex_ = -1, int tileID_ = -1):
            name(name_), pos(pos_), w(w_), h(h_), tilesetIndex(tilesetIndex_), tileID(tileID_), texture(texture_) {};

        int get_tilesetIndex() const { return tilesetIndex; }
        int get_tileID() const { return tileID; }
        ALLEGRO_BITMAP* get_texture() const { return texture; }
        Point* get_pos() { return &pos; }
        float get_w() const { return w; }
        float get_h() const { return h; }

        void set_tilesetIndex(int i) { tilesetIndex = i; }
        void set_tileID(int id) { tileID = id; }
        void set_texture(ALLEGRO_BITMAP* tex) { texture = tex; }
            
        virtual void draw(GameMap& gmap, const Point* target_pos);
        virtual void update(GameMap& gmap, const ALLEGRO_KEYBOARD_STATE& k_state = {}, const ALLEGRO_MOUSE_STATE& m_state = {}) {};
};

#endif