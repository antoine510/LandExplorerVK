#pragma once

#include "vector.h"
#include <SDL.h>

namespace Constant {
constexpr float pi = 3.1415926535f;
constexpr float deg2rad(float deg) { return deg * pi / 180.0f; };
constexpr float rad2deg(float rad) { return rad / pi * 180.0f; };
}


typedef struct PerlinNoise {
	float* g;
	int width;
	int stepCount;
} PerlinNoise;

typedef struct PerlinNoise2D {
	Vec2* g;
	int width, height;
	int stepCountX, stepCountY;
} PerlinNoise2D;

constexpr int addAbs(int base, int add) { return (base < 0) ? base - add : base + add; }
inline int exclusive(float x) { return (x >(int)x) ? (int)x : (int)x - 1; }
inline Uint32 getDistance(Uint32 x1, Uint32 y1, Uint32 x2, Uint32 y2) { return std::abs((int32_t)x2 - (int32_t)x1) + std::abs((int32_t)y2 - (int32_t)y1); }
SDL_Rect centeredRect(SDL_Rect* base, bool centerW, bool centerH);
SDL_Color modulateColor(SDL_Color* base, float rmod, float gmod, float bmod);

void clampRect(SDL_Rect* rect, int xmin, int ymin, int xmax, int ymax);
void clampWorld(Uint16 wTerrain, Uint16 hTerrain, SDL_Rect* rect, int border);
void clampWorldChunck(Uint16 wTerrainChunck, Uint16 hTerrainChunck, SDL_Rect* rect, int border);

void rotateClockwise(SDL_Surface* src, SDL_Rect* srcRect, SDL_Surface* dst, SDL_Rect* dstRect, Uint8 colMod);
void rotateCounterclockwise(SDL_Surface* src, SDL_Rect* srcRect, SDL_Surface* dst, SDL_Rect* dstRect, Uint8 colMod);
void rotateHalf(SDL_Surface* src, SDL_Rect* srcRect, SDL_Surface* dst, SDL_Rect* dstRect, Uint8 colMod);

PerlinNoise initPerlin(int width, int scale);
float perlinNoise(PerlinNoise perlin, int xBlock);

PerlinNoise2D initPerlin2D(int width, int height, int scaleX, int scaleY);
float perlinNoise2D(PerlinNoise2D perlin, int xBlock, int yBlock);
void savePerlinNoise2D(PerlinNoise2D perlin2D);
int getPosPerlin2D(PerlinNoise2D* perlin, int x, int y);
void destroyPerlinNoise2D(PerlinNoise2D perlin2D);

