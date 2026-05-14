#ifndef IncludeWorkbench
#define IncludeWorkbench

#include <vector>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <string>

class Item;
struct GameMap;

struct CraftRecipe {
    // 9 item names in int, "" = empty
    std::string grid[9];
    std::string resultName;
    int         resultCount = 1;

    //constructor for easy initialization
    CraftRecipe(std::initializer_list<std::string> g,
                std::string result, int count = 1)
        : resultName(std::move(result)), resultCount(count)
    {
        int i = 0;
        for (auto& s : g) {
            if (i >= 9) break;
            grid[i++] = s;
        }
    }
};

struct WorkbenchCell {
    Item* item     = nullptr;
    int   quantity = 0;
    bool  isEmpty() const { return item == nullptr || quantity == 0; }
};


class Workbench {
    private:
        WorkbenchCell craft_grid[9];   // 3×3 row order

        // new result every time craft_grid changes
        Item* result_item    = nullptr;
        int   result_count   = 0;

        bool  open           = false;

        float drawX = 0.f;
        float drawY = 0.f;

        ALLEGRO_BITMAP* bgImage = nullptr; // workbench_UI.png
        ALLEGRO_BITMAP* spriteImage = nullptr; // workbench.png on the ground
        ALLEGRO_FONT*   font    = nullptr;
        int width = 0;
        int height = 0;


        static std::vector<CraftRecipe> recipes;  // one copy for each object
        static bool recipesLoaded;

        void  loadRecipes();
        void  updateResult();          // call after every grid change
        int   gridCellAtPoint(float sx, float sy) const;
        bool  resultSlotAtPoint(float sx, float sy) const;
        int   inventorySlotAtPoint(float sx, float sy, GameMap& gm) const;

        Item* buildResultItem(const std::string& name, int count) const;

        // returns everything in craft_grid back to inventory or drops it.
        void  returnCraftGridToInventory(GameMap& gm);

    public:
        static const int CRAFT_OFFSET_X   = 105;
        static const int CRAFT_OFFSET_Y   = 36;
        static const int RESULT_OFFSET_X  = 352; 
        static const int RESULT_OFFSET_Y  = 79; 

        // world pos in pixels
        float worldX = 0.f;
        float worldY = 0.f;
        Workbench(float wx, float wy,
                ALLEGRO_BITMAP* bg  = al_load_bitmap("images/storage/workbench/workbench_UI.png"),
                ALLEGRO_BITMAP* spr = al_load_bitmap("images/storage/workbench/workbench.png"),
                ALLEGRO_FONT*   fnt = al_create_builtin_font());
        ~Workbench();
        
        // ground worbench's sprite
        void drawSprite(float camX, float camY) const;

        // UI window when open
        void draw(GameMap& gm) const;

        bool handleClick(float sx, float sy, bool rightButton, GameMap& gm);
        void update(GameMap& gm);

        bool isOpen()    const { return open; }
        void setOpen(bool v)   { open = v; }

        void openUI();

        void closeUI(GameMap& gm);
};

#endif
