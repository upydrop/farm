g++ main.cpp engine.cpp items/Item.cpp storage/Inventory.cpp funcs/mapFuncs.cpp sprite/Player.cpp sprite/Sprite.cpp funcs/eventHandler.cpp consts.cpp crops/Crop.cpp crops/Wheat.cpp funcs/LootHandler.cpp items/FishingRod.cpp storage/Bag.cpp ^
src/Map.cpp src/TileLayer.cpp src/ObjectGroup.cpp src/LayerGroup.cpp ^
src/ImageLayer.cpp src/Object.cpp src/Property.cpp src/ObjectTypes.cpp ^
src/TileSet.cpp src/FreeFuncs.cpp src/miniz.c src/detail/pugixml.cpp ^
-o bin/app -std=c++17 ^
-I. -Isrc -Iallegro/include ^
-Lallegro/lib -lallegro -lallegro_main -lallegro_primitives -lallegro_image -lallegro_font -lallegro_ttf