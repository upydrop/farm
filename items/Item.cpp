#include "../items/Item.hpp"
#include "../funcs/mapFuncs.hpp"
#include "../crops/Wheat.hpp"

Item::Item(string name_, string img_path_, Point pos_, int healthPoints_, float use_cooldown_, bool stackable_){
    name = name_;
    pos = pos_;
    stackable = stackable_;
    healthPoints = healthPoints_;
    use_cooldown = use_cooldown_;
    img_path = img_path_;
    img = al_load_bitmap(img_path_.c_str());
    if (!img) throw "Failed to load item image";
    w = al_get_bitmap_width(img);
    h = al_get_bitmap_height(img);
}

void Item::draw(Point pos_, int new_w, int new_h, std::optional<std::reference_wrapper<GameMap>> gmap){ // new_w and new_h are optional parameters for scaling the image. Used when the item is dropped on the ground
     if (img) {
        float draw_w = (new_w > 0) ? new_w : al_get_bitmap_width(img);
        float draw_h = (new_h > 0) ? new_h : al_get_bitmap_height(img);
        Point p = (pos_.x < 0 && pos_.y < 0) ? pos : pos_;
        al_draw_scaled_bitmap(img, 0, 0, w, h, p.x, p.y, draw_w, draw_h, 0);
    }
}

Item::~Item(){
    if (img) {
        al_destroy_bitmap(img);
    }
}

bool Item::functionality(GameMap& gmap){
    return false;
}

Item::Item(const Item& other)
    : name(other.name),
      pos(other.pos),
      img_path(other.img_path),
      stackable(other.stackable),
      healthPoints(other.healthPoints),
      use_cooldown(other.use_cooldown),
      last_used(other.last_used),
      w(other.w),
      h(other.h)
{
    if (!other.img_path.empty()) {
        img = al_load_bitmap(img_path.c_str());
    } else {
        img = nullptr;
    }
}

bool Hoe::functionality(GameMap& gmap){
    string state = gmap.player->get_state();
    Point pos = *gmap.player->get_pos();
    if (state == "right") { 
        pos.x += TILESIZE/2 + gmap.player->get_w();
        pos.y += gmap.player->get_h();
    }
    else if (state == "left") { 
        pos.x -= TILESIZE/2;
        pos.y += gmap.player->get_h();
    } 
    else if (state == "up") {
        pos.x += gmap.player->get_w()/2;
    }
    else if (state == "down") { 
        pos.y += TILESIZE/2 + gmap.player->get_h(); 
        pos.x += gmap.player->get_w()/2;
    }
    vector<TileID> tileIDs = getTileIndex(gmap, floor(pos.x/TILESIZE), floor(pos.y/TILESIZE));
    if (tileIDs.size() <= 0) return false;
    for (const auto& tileID : tileIDs){
        Tile& tile = gmap.tileLayers[tileID.layerID][tileID.tileID];
        tileType plowed = tileTypes["plowed"];
        tileType grass = tileTypes["grass"];
        if ((tile.id == grass.tileID && tile.tilesetIndex == grass.tilesetIndex)){
            setTile(gmap, tileID.layerID, tileID.tileID, plowed.tileID, plowed.tilesetIndex);
            change_healthPoints(-1);
            update_buffer(gmap);
            gmap.player->set_current_state(CharacterState::Tilling);
            if (get_healthPoints() <= 0){
                return true;
            }
            return false;
        }
    }
    return false;
}

bool WheatSeed::functionality(GameMap& gmap){
    string state = gmap.player->get_state();
    Point pos = *gmap.player->get_pos();
    if (state == "right") { 
        pos.x += TILESIZE/2 + gmap.player->get_w();
        pos.y += gmap.player->get_h();
    }
    else if (state == "left") { 
        pos.x -= TILESIZE/2;
        pos.y += gmap.player->get_h();
    } 
    else if (state == "up") {
        pos.x += gmap.player->get_w()/2;
    }
    else if (state == "down") { 
        pos.y += TILESIZE/2 + gmap.player->get_h(); 
        pos.x += gmap.player->get_w()/2;
    }
    for (int i = 0; i < gmap.crops.size(); ++i) {
        if (gmap.crops[i]->getTileX() == floor(pos.x/TILESIZE) && gmap.crops[i]->getTileY() == floor(pos.y/TILESIZE)) {
            return false; // already a crop planted here
        }
    }
    vector<TileID> tileIDs = getTileIndex(gmap, floor(pos.x/TILESIZE), floor(pos.y/TILESIZE));
    if (tileIDs.size() <= 0) return false;
    for (const auto& tileID : tileIDs){
        Tile& tile = gmap.tileLayers[tileID.layerID][tileID.tileID];
        tileType plowed = tileTypes["plowed"];
        tileType watered_plowed = tileTypes["watered_plowed"];
        if ((tile.id == watered_plowed.tileID && tile.tilesetIndex == watered_plowed.tilesetIndex)){
            unique_ptr<Crop> new_crop = std::make_unique<Wheat>(floor(pos.x/TILESIZE), floor(pos.y/TILESIZE));
            if (tile.id != watered_plowed.tileID){
                new_crop->setOnWateredTile(false);
            } else {
                new_crop->setOnWateredTile(true);
            }
            gmap.crops.push_back(std::move(new_crop));
            return true; // seed is consumed on use
        }
    }
    return false;
};

bool WateringCan::functionality(GameMap& gmap){
    if (water_level <= 0) return false;
    string state = gmap.player->get_state();
    Point pos = *gmap.player->get_pos();
    if (state == "right") { 
        pos.x += TILESIZE/2 + gmap.player->get_w();
        pos.y += gmap.player->get_h();
    }
    else if (state == "left") { 
        pos.x -= TILESIZE/2;
        pos.y += gmap.player->get_h();
    } 
    else if (state == "up") {
        pos.x += gmap.player->get_w()/2;
    }
    else if (state == "down") { 
        pos.y += TILESIZE/2 + gmap.player->get_h(); 
        pos.x += gmap.player->get_w()/2;
    }
    vector<TileID> tileIDs = getTileIndex(gmap, floor(pos.x/TILESIZE), floor(pos.y/TILESIZE));
    if (tileIDs.size() <= 0) return false;
    for (const auto& tileID : tileIDs){
        Tile& tile = gmap.tileLayers[tileID.layerID][tileID.tileID];
        tileType plowed = tileTypes["plowed"];
        tileType watered_plowed = tileTypes["watered_plowed"];
        if ((tile.id == plowed.tileID && tile.tilesetIndex == plowed.tilesetIndex) || (tile.id == watered_plowed.tileID && tile.tilesetIndex == watered_plowed.tilesetIndex)){
            if (tile.id != watered_plowed.tileID){
                setTile(gmap, tileID.layerID, tileID.tileID, watered_plowed.tileID, watered_plowed.tilesetIndex);
                update_buffer(gmap);
                water_level -= water_usage;
            }
            for (const auto& crop : gmap.crops) {
                if (crop->getTileX() == floor(pos.x/TILESIZE) && crop->getTileY() == floor(pos.y/TILESIZE)) {
                    crop->water(water_usage);
                    crop->setOnWateredTile(true);
                    if (tile.id == watered_plowed.tileID){
                        water_level -= water_usage; // if tile already watered, yet plant is not
                    }
                    break;
                }
            }
            return false;
        }
    }
    return false;
};

bool Bread::functionality(GameMap &gmap){
    gmap.player->change_hungerPoints(3);
    return true;
}