#include "caves.h"
#include "terrain.h"
#include "utility/mathUtility.h"

static int caveNoiseHeight;

static int getPosCaves(int x, int y) {return x*caveNoiseHeight + y;}
static void fillRandom(char* noise, int width, int height, PerlinNoise2D perlin2D);
//static int surroundings(char* noise, int posX, int posY);
//static void automatonStep(char* noise, int width, int height);

void fillRandom(char* noise, int width, int height, PerlinNoise2D perlin2D)
{
	int x, y;
	for(x = 0; x < width; x++)
    {
        for(y = 1; y < height-1; y++)
        {
            float seuil = (float)y / height * 0.2f - 0.5f;
            noise[getPosCaves(x, y)] = perlinNoise2D(perlin2D, x, y) > seuil;
        }
        noise[getPosCaves(x, 0)] = 1;
		noise[getPosCaves(x, height-1)] = 1;
    }
	for(y = 1; y < height-1; y++) {
		noise[getPosCaves(0, y)] = 1;
		noise[getPosCaves(width-1, y)] = 1;
	}
}

/*int surroundings(char* noise, int posX, int posY)
{
	int sum = noise[getPosCaves(posX-1, posY-1)];
	sum += noise[getPosCaves(posX-1, posY)];
	sum += noise[getPosCaves(posX-1, posY+1)];
	sum += noise[getPosCaves(posX, posY-1)];
	sum += noise[getPosCaves(posX, posY+1)];
	sum += noise[getPosCaves(posX+1, posY-1)];
	sum += noise[getPosCaves(posX+1, posY)];
	sum += noise[getPosCaves(posX+1, posY+1)];

	return sum;
}*/

/*void automatonStep(char* noise, int width, int height)
{
	int i, j;
	for(i = 1; i < width-1; i++)
	{
		for(j = 1; j < height-1; j++)
		{
			int entour = surroundings(noise, i, j);
			if(noise[getPosCaves(i, j)]) {
				if(entour < 3)
				{
					noise[getPosCaves(i, j)] = 0;
				}
			}
			else {
				if(entour >= 6)
				{
					noise[getPosCaves(i, j)] = 1;
				}
			}
		}
	}
}*/

void generateCaves(Terrain* terrain)
{
	char* noise = (char*)malloc(terrain->width * terrain->height * sizeof(char));
	caveNoiseHeight = terrain->height;

	#ifdef PERFLOG
        int t1 = SDL_GetTicks();
	#endif // PERFLOG

	PerlinNoise2D perlin2D = initPerlin2D(terrain->width, terrain->height, 8, 16);
	fillRandom(noise, terrain->width, terrain->height , perlin2D);

	int x, y;
	for(x = TERRAIN_BORDER; x < terrain->width - TERRAIN_BORDER; x++)
    {
        for(y = TERRAIN_BORDER; y < terrain->height - TERRAIN_BORDER; y++)
        {
            if(getBlock(terrain, x, y).type != 0 && noise[getPosCaves(x, y)] == 0) getBlockPtr(terrain, x, y)->type = 0;
        }
    }

	int i;
	for(i = 0; i < CELLULAR_STEPS; i++)
	{
		//etapeAutomate(noise);
	}
	//savePerlinNoise2D(perlin2D);
	#ifdef PERFLOG
        int t2 = SDL_GetTicks();
        printf("Caves generation : %d\n", t2 - t1);
	#endif // PERFLOG

	destroyPerlinNoise2D(perlin2D);
	free(noise);
}
