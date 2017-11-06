#pragma once

#include "oglCommon.h"
#include <SDL_ttf.h>
#include "renderer.h"
#include "shader.h"
#include "utility/vector.h"

typedef struct Texture
{
    GLuint id;

    int w, h;
    float xscale, yscale;
    float angle;
    SDL_Color* colorMod;
    float alphaMod;
    int clipW, clipH;
	Vec2 clipOffset;

    GLenum internalFormat, format;
    OGLRenderer* renderer;
    ShaderType type;
    float xGL, yGL, wGL, hGL;

    GLuint VBOid;
    GLuint VAOid;
} Texture;

Texture* createTextureFromFile(OGLRenderer* renderer, const char* image);
Texture* createTextureFromSurface(OGLRenderer* renderer, SDL_Surface* surface);
Texture* createTextureFromText(OGLRenderer* renderer, const char* text, int outline, SDL_Color color, TTF_Font* font);
Texture* createEmptyTexture(OGLRenderer* renderer, int width, int height);

void updateTextureFromSurface(Texture* texture, SDL_Surface* surface);
void updateTexture(Texture* texture, const SDL_Rect* rect, const void* pixels, int pitch);
void modifyTextureFromSurface(Texture* texture, SDL_Surface* surface);
void modifyTexture(Texture* texture, const SDL_Rect* rect, const void* pixels, int pitch);
void setTextureColorMod(Texture* texture, SDL_Color* color);
void setTextureAlphaMod(Texture* texture, Uint8 alpha);
void setTextureAngle(Texture* texture, float angle);
void setTextureOrigin(Texture* texture, int x, int y);
void setTextureOriginRatio(Texture* texture, float x, float y);
void setTexutreScalingMethod(Texture* texture, GLint scalingMethod);
void setTextureClipSize(Texture* texture, int clipW, int clipH);
void setTextureClip(Texture* texture, int clipX, int clipY);
void setTexturePos(Texture* texture, int posX, int posY);
void setTextureScale(Texture* texture, float scale);
void setTextureSize(Texture* texture, int w, int h);
void setTextureFullscreen(Texture* texture);
void setTextureFullscreenCrop(Texture* texture);
void drawTexture(Texture* texture);

void invertPixels(SDL_Surface* srf);

void destroyTexture(Texture* texture);

int getTextureBPP(Texture* tex);

