# 🌾 Farm

A small 2D farming game built with **C++ and Allegro 5**. Explore the map, grow crops, collect items, craft recipes, and go fishing.
Not an end-use product😊

## 📸 Preview
<img width="711" height="452" alt="image" src="https://github.com/user-attachments/assets/f6f7ef92-0371-486f-9cfe-a2e079e4dac1" />

## 🛠️ Tech Stack

- **C++17** — game logic and object-oriented architecture
- **Allegro 5** — rendering, input, fonts, and game loop
- **tmxlite** — loading Tiled `.tmx` maps (/src folder)
- **Custom systems** — inventory, item interactions, crop growth, fishing, and crafting

## ✨ Interesting Features

- 🌱 **Crop system** — plant, grow, and harvest different crops.
- 🎒 **Inventory & storage** — manage item stacks, collect loot, and organize items in a bag.
- 🔨 **Crafting** — recipes are defined in `storage/Workbench.cpp` and matched against a 3×3 crafting grid.
- 🎣 **Fishing** — cast a bobber into water, wait for a catch, and reel it back in.
- 🗺️ **Tiled maps** — load map data and collision information from `.tmx` files.

## 🚀 How to Run

1. Install **MinGW-w64 (g++)** and configure **Allegro 5**.
2. Make sure the Allegro DLLs are available through your system's `PATH`.
3. Run `build.bat` from the project root.
4. Launch the compiled game:

```bash
bin/app.exe
```

Run the game from the project root so that relative paths to `images/` and `data/` work correctly.

## 🎮 Controls

| Key / Input | Action |
|---|---|
| `W A S D` / Arrow keys | Move |
| `Shift` | Run |
| `T` | Open / close bag |
| `F` | Open / close nearby workbench |
| `E` | Pick up nearby items |
| `Q` | Drop the selected item |
| `Esc` | Close open UI |
| Left click | Use selected item, harvest crops, or interact with UI |
| Right click | Secondary interaction with inventory / crafting UI |

**Fishing:** Select the fishing rod and left-click a water tile to cast.

---

*Happy farming!* 🌻
