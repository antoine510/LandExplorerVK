#pragma once

#define BLOC_TYPE_COUNT 256

#define TERRAIN_WIDTH 2048
#define TERRAIN_HEIGHT 1024

#define CHUNCK_SIZE 16
#define CHUNCK_BLOCS (CHUNCK_SIZE * CHUNCK_SIZE)

#define TERRAIN_BORDER 16
#define TERRAIN_BORDER_CHUNCK (TERRAIN_BORDER / CHUNCK_SIZE)

#define BLOC_NONE 0
#define BLOC_DIRT 1
#define BLOC_ROCK 2
#define BLOC_WATER 3
#define BLOC_BRICK 4
#define BLOC_WOOD 5
#define BLOC_TORCH 6
#define BLOC_GRASS 7
#define BLOC_SAND 8

#define BACKWALL_NONE 0
#define BACKWALL_DIRT 1
#define BACKWALL_ROCK 2

struct Bloc {
    Uint8 type;
    Uint8 light;

	constexpr Bloc* left() { return this - 1; }
	constexpr Bloc* right() { return this + 1; }
	constexpr Bloc* up() { return this - TERRAIN_WIDTH; }
	constexpr Bloc* down() { return this + TERRAIN_WIDTH; }
};

