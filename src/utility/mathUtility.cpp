#include "mathUtility.h"
#include "terrain/terrain.h"
#include "utility/random.h"

void savePerlinNoise2D(PerlinNoise2D perlin2D);

SDL_Rect rectOriginRatio(SDL_Rect base, float rX, float rY) {
	base.x -= int(rX * base.w); base.y -= int(rY * base.h);
	return base;
}


void clampRect(SDL_Rect* rect, int xmin, int ymin, int xmax, int ymax) {
	//Precondition : the rectangle is at least in part in the clamping zone
	if(rect->x < xmin) { rect->w += rect->x - xmin; rect->x = xmin; }
	if(rect->y < ymin) { rect->h += rect->y - ymin; rect->y = ymin; }
	if(rect->x + rect->w > xmax) { rect->w = xmax - rect->x; }
	if(rect->y + rect->h > ymax) { rect->h = ymax - rect->y; }
}

PerlinNoise initPerlin(int width, int scale) {
	PerlinNoise perlin;
	perlin.width = width;
	perlin.stepCount = width / scale;
	perlin.g = (float*)malloc((perlin.stepCount + 1) * sizeof(float));
	int i;
	for(i = 0; i <= perlin.stepCount; i++) perlin.g[i] = Random<float>::range(-1.f, 1.f);

	return perlin;
}

float perlinNoise(PerlinNoise perlin, int xBlock) {
	float x = (float)xBlock / (perlin.width) * perlin.stepCount;
	int x0 = (int)x, x1 = x0 + 1;

	float u = perlin.g[x0] * (x - x0), v = perlin.g[x1] * (x - x1);

	return ((u + (3 * (x - x0)*(x - x0) - 2 * (x - x0)*(x - x0)*(x - x0))*(v - u)) + 1.0f) / 2.0f;
}

PerlinNoise2D initPerlin2D(int width, int height, int scaleX, int scaleY) {
	PerlinNoise2D perlin;
	perlin.stepCountX = width / scaleX;
	perlin.stepCountY = height / scaleY;
	perlin.width = width;
	perlin.height = height;
	perlin.g = (Vec2*)malloc((perlin.stepCountX + 1) * (perlin.stepCountY + 1) * sizeof(Vec2));
	int i;
	for(i = 0; i < (perlin.stepCountX + 1) * (perlin.stepCountY + 1); i++) {
		float theta = Random<float>::range(0, 2 * Constant::pi);
		perlin.g[i].x = std::cos(theta);
		perlin.g[i].y = std::sin(theta);
	}

	return perlin;
}

float perlinNoise2D(PerlinNoise2D perlin, int xBlock, int yBlock) {
	float x = (float)xBlock / perlin.width * perlin.stepCountX;
	float y = (float)yBlock / perlin.height * perlin.stepCountY;
	int x0 = (int)x, x1 = (int)x + 1;
	int y0 = (int)y, y1 = (int)y + 1;

	float u = glm::dot(perlin.g[getPosPerlin2D(&perlin, x0, y0)], Vec2(x - x0, y - y0));
	float v = glm::dot(perlin.g[getPosPerlin2D(&perlin, x1, y0)], Vec2(x - x1, y - y0));
	float s = glm::dot(perlin.g[getPosPerlin2D(&perlin, x0, y1)], Vec2(x - x0, y - y1));
	float t = glm::dot(perlin.g[getPosPerlin2D(&perlin, x1, y1)], Vec2(x - x1, y - y1));

	float avg1 = u + (3 * (x - x0)*(x - x0) - 2 * (x - x0)*(x - x0)*(x - x0))*(v - u);
	float avg2 = s + (3 * (x - x0)*(x - x0) - 2 * (x - x0)*(x - x0)*(x - x0))*(t - s);

	return avg1 + (3 * (y - y0)*(y - y0) - 2 * (y - y0)*(y - y0)*(y - y0))*(avg2 - avg1);
}

void savePerlinNoise2D(PerlinNoise2D perlin2D) {
	SDL_Surface* srf = SDL_CreateRGBSurface(0, perlin2D.width, perlin2D.height, 24, 0xff0000, 0x00ff00, 0x0000ff, 0);
	if(srf == NULL) printf("Couldn't create surface");
	Uint8* px = (Uint8*)srf->pixels;
	int x, y;
	for(y = 0; y < perlin2D.height; y++) {
		for(x = 0; x < perlin2D.width; x++) {
			float bruit = (perlinNoise2D(perlin2D, x, y) + 1.0f) / 2;
			Uint8 bruitNorm = (Uint8)(bruit * 255);
			px[y * srf->pitch + x * 3] = bruitNorm;
			px[y * srf->pitch + x * 3 + 1] = bruitNorm;
			px[y * srf->pitch + x * 3 + 2] = bruitNorm;
		}
	}
	SDL_SaveBMP(srf, "test.bmp");
}

int getPosPerlin2D(PerlinNoise2D* perlin, int x, int y) { return x * (perlin->stepCountY + 1) + y; }

void destroyPerlinNoise2D(PerlinNoise2D perlin2D) {
	free(perlin2D.g);
}


