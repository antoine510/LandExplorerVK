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
SDL_Rect rectOriginRatio(SDL_Rect base, float rX, float rY);
inline Vec4 colorToVec(SDL_Color c) { return Vec4(c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, c.a / 255.0f); }

void clampRect(SDL_Rect* rect, int xmin, int ymin, int xmax, int ymax);
inline void clampWorld(Uint16 wTerrain, Uint16 hTerrain, SDL_Rect* rect, int border) { clampRect(rect, border, border, wTerrain - border, hTerrain - border); }

PerlinNoise initPerlin(int width, int scale);
float perlinNoise(PerlinNoise perlin, int xBlock);

PerlinNoise2D initPerlin2D(int width, int height, int scaleX, int scaleY);
float perlinNoise2D(PerlinNoise2D perlin, int xBlock, int yBlock);
int getPosPerlin2D(PerlinNoise2D* perlin, int x, int y);
void destroyPerlinNoise2D(PerlinNoise2D perlin2D);

