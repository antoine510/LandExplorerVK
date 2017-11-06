#include "texture.h"
#include "../displayInfo.h"
#include "utility/mathUtility.h"
#include <SDL_image.h>

#define VERTICES_SIZE (12 * sizeof(float))
#define TEXCOORDS_SIZE (12 * sizeof(float))

static Texture* createDefaultTexture(OGLRenderer* renderer, int width, int height, GLenum internalFormat, GLenum format);

Texture* createTextureFromFile(OGLRenderer* renderer, const char* image)
{
    SDL_Surface* surface = IMG_Load(image);
    if(surface == NULL) fprintf(stderr, "Error : No texture named %s", image);
    //inverserPixels(surface);

    return createTextureFromSurface(renderer, surface);
}

Texture* createTextureFromSurface(OGLRenderer* renderer, SDL_Surface* surface)
{
    GLenum internalFormat, format;
    if(surface->format->BytesPerPixel == 3)     //RGB
    {
        internalFormat = GL_RGB8;
        format = (surface->format->Rmask == 0x000000ff) ? GL_RGB : GL_BGR;
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }
    else        //RGBA
    {
        internalFormat = GL_RGBA8;
        format = (surface->format->Rmask == 0x000000ff) ? GL_RGBA : GL_BGRA;
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    }

    Texture* texture = createDefaultTexture(renderer, surface->w, surface->h, internalFormat, format);

    //float vertices[] = {-1.0f, 1.0f, texture->wGL-1.0f, 1.0f,  texture->wGL-1.0f, 1.0f-texture->hGL,
    //                    -1.0f, 1.0f, -1.0f, 1.0f-texture->hGL, texture->wGL-1.0f, 1.0f-texture->hGL};
	float vertices[] = { 0.0f, 0.0f,			texture->wGL, 0.0f,
												texture->wGL, texture->hGL,
	                    0.0f, 0.0f,
						0.0f, texture->hGL,		texture->wGL, texture->hGL };
    float texCoords[] = {0, 1,					1, 1,
												1, 0,
						 0, 1,
						 0, 0,					1, 0};

    glGenTextures(1, &texture->id);
    glBindTexture(GL_TEXTURE_2D, texture->id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, texture->internalFormat, texture->w, texture->h, 0, texture->format, GL_UNSIGNED_BYTE, surface->pixels);

    glBindTexture(GL_TEXTURE_2D, 0);

    createVBOandVAO(&texture->VBOid, &texture->VAOid, vertices, texCoords, 12);

    return texture;
}

Texture* createTextureFromText(OGLRenderer* renderer, const char* text, int outline, SDL_Color color, TTF_Font* font)
{
    Texture* res;

    SDL_Surface* textSrf = TTF_RenderText_Blended(font, text, color);
    if(outline != 0)
    {
        SDL_Color outlineColor = {0, 0, 0, 255};

        TTF_SetFontOutline(font, outline);
        SDL_Surface* outline = TTF_RenderText_Blended(font, text, outlineColor);
        TTF_SetFontOutline(font, 0);
        SDL_BlitSurface(textSrf, NULL, outline, NULL);

        res = createTextureFromSurface(renderer, outline);

        SDL_FreeSurface(outline);
    }
    else
    {
        res = createTextureFromSurface(renderer, textSrf);
    }
    SDL_FreeSurface(textSrf);

    return res;
}

Texture* createEmptyTexture(OGLRenderer* renderer, int width, int height)
{
    Texture* texture = createDefaultTexture(renderer, width, height, GL_RGBA8, GL_BGRA);

    /*float vertices[] = {-1.0f, 1.0f, texture->wGL-1.0f, 1.0f,  texture->wGL-1.0f, 1.0f-texture->hGL,
                        -1.0f, 1.0f, -1.0f, 1.0f-texture->hGL, texture->wGL-1.0f, 1.0f-texture->hGL};
    float texCoords[] = {0, 0, 1, 0, 1, 1,    0, 0, 0, 1,   1, 1};*/
	float vertices[] = {	0.0f, 0.0f,				texture->wGL, 0.0f,
													texture->wGL, texture->hGL,
							0.0f, 0.0f,
							0.0f, texture->hGL,		texture->wGL, texture->hGL };
	float texCoords[] = {	0, 1,					1, 1,
													1, 0,
							0, 1,
							0, 0,					1, 0 };

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    glGenTextures(1, &texture->id);
    glBindTexture(GL_TEXTURE_2D, texture->id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, texture->internalFormat, width, height, 0, texture->format, GL_UNSIGNED_BYTE, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    createVBOandVAO(&texture->VBOid, &texture->VAOid, vertices, texCoords, 12);

    return texture;
}

Texture* createDefaultTexture(OGLRenderer* renderer, int width, int height, GLenum internalFormat, GLenum format)
{
    Texture* texture = malloc(sizeof(Texture));

    texture->id = 0;
    texture->type = shader_base;
    texture->renderer = renderer;
    texture->w = width; texture->h = height;
    texture->colorMod = NULL;
    texture->alphaMod = 1.0f;
    texture->angle = 0.0f;
    texture->xscale = 1.0f;
	texture->yscale = 1.0f;
    texture->clipW = width; texture->clipH = height;
	texture->clipOffset = (Vect2) {0.0f, 0.0f};
    texture->internalFormat = internalFormat;
    texture->format = format;
    texture->xGL = 0.0f; texture->yGL = 0.0f;
    texture->wGL = (float)width / myDisplayMode->w *2;
    texture->hGL = (float)height / myDisplayMode->h *2;
    texture->VBOid = 0;
    texture->VAOid = 0;

    return texture;
}

void updateTextureFromSurface(Texture* texture, SDL_Surface* surface)
{
    SDL_Rect rect = {0, 0, surface->w, surface->h};
    updateTexture(texture, &rect, surface->pixels, surface->pitch);
}

void updateTexture(Texture* texture, const SDL_Rect* rect, const void* pixels, int pitch)
{
    if(texture == NULL) return;
    glBindTexture(GL_TEXTURE_2D, texture->id);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, pitch / getTextureBPP(texture));
        glTexSubImage2D(GL_TEXTURE_2D, 0, rect->x, rect->y, rect->w, rect->h, texture->format, GL_UNSIGNED_BYTE, pixels);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void modifyTextureFromSurface(Texture* texture, SDL_Surface* surface)
{
    SDL_Rect rect = {0, 0, surface->w, surface->h};
    modifyTexture(texture, &rect, surface->pixels, surface->pitch);
}

void modifyTexture(Texture* texture, const SDL_Rect* rect, const void* pixels, int pitch)
{
    if(texture == NULL) return;
    glBindTexture(GL_TEXTURE_2D, texture->id);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, pitch / getTextureBPP(texture));
        glTexImage2D(GL_TEXTURE_2D, 0, texture->internalFormat, rect->w, rect->h, 0, texture->format, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
    texture->w = rect->w; texture->h = rect->h;
    setTextureSize(texture, rect->w, rect->h);
    texture->clipW = texture->w; texture->clipH = texture->h;
}

void setTextureColorMod(Texture* texture, SDL_Color* color)
{
    texture->colorMod = color;
}

void setTextureAlphaMod(Texture* texture, Uint8 alpha)
{
    texture->alphaMod = (float)alpha / 255;
}

void setTextureAngle(Texture* texture, float angle)
{
    texture->angle = angle;
}

void setTextureOrigin(Texture* texture, int x, int y)
{
	setTextureOriginRatio(texture, (float)x / texture->w, (float)y / texture->h);
}

void setTextureOriginRatio(Texture* texture, float x, float y)
{
	float rx = x * texture->wGL, ry = y * texture->hGL;
	float vertices[] = {-rx, -ry,					texture->wGL - rx, -ry,
													texture->wGL - rx, texture->hGL - ry,
						-rx, -ry,
						-rx, texture->hGL - ry,		texture->wGL - rx, texture->hGL - ry };
	updateVBO(texture->VBOid, vertices, NULL, 12);
}

void setTexutreScalingMethod(Texture* texture, GLint scalingMethod)
{
    glBindTexture(GL_TEXTURE_2D, texture->id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, scalingMethod);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, scalingMethod);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void setTextureClipSize(Texture* texture, int clipW, int clipH)
{
    texture->clipW = clipW;
    texture->clipH = clipH;
    texture->wGL = (float)clipW / myDisplayMode->w *2;
    texture->hGL = (float)clipH / myDisplayMode->h *2;
	float clipWratio = (float)texture->clipW / texture->w, clipHratio = (float)texture->clipH / texture->h;

	float vertices[] = { 0.0f, 0.0f,				texture->wGL, 0.0f,
													texture->wGL, texture->hGL,
						 0.0f, 0.0f,
						 0.0f, texture->hGL,		texture->wGL, texture->hGL };
	float texCoords[] = {	0, clipHratio,	clipWratio, clipHratio,
											clipWratio, 0,
							0, clipHratio,
							0, 0,			clipWratio, 0 };

	updateVBO(texture->VBOid, vertices, texCoords, 12);
}

void setTextureClip(Texture* texture, int clipX, int clipY)
{
	texture->clipOffset.x = (float)texture->clipW * clipX / texture->w;
	texture->clipOffset.y = (float)texture->clipH * clipY / texture->h;
    /*float y1 = (float)texture->clipH * clipY / texture->h;
    float x2 = x1 + (float)texture->clipW / texture->w, y2 = y1 + (float)texture->clipH / texture->h;
    float texCoords[] = {x1, y2, x2, y2,
								x2, y1,
						x1, y2,
						x1, y1, x2, y1};
    updateVBO(texture->VBOid, NULL, texCoords, 12);*/
}

void setTexturePos(Texture* texture, int posX, int posY)
{
    texture->xGL = (float)posX / myDisplayMode->w *2;
    texture->yGL = (float)posY / myDisplayMode->h *2;
}

void setTextureScale(Texture* texture, float scale)
{
    texture->xscale = scale;
	texture->yscale = scale;
}

void setTextureSize(Texture* texture, int w, int h)
{
	texture->xscale = (float)w / texture->clipW;
	texture->yscale = (float)h / texture->clipH;
}

void setTextureFullscreen(Texture* texture)
{
	setTextureOriginRatio(texture, 0.5f, 0.5f);
	setTextureSize(texture, myDisplayMode->w, myDisplayMode->h);
	setTexturePos(texture, 0, 0);
}

void setTextureFullscreenCrop(Texture* texture)
{
    float scale = maxf((float)myDisplayMode->w / texture->clipW, (float)myDisplayMode->h / texture->clipH);
	setTextureOriginRatio(texture, 0.5f, 0.5f);
    setTextureScale(texture, scale);
    setTexturePos(texture, 0, 0);
}

void drawTexture(Texture* texture)
{
    if(texture->type == shader_base)
    {
		setBaseUniformTransform((BaseShaderUniforms*)texture->renderer->baseShader->uniforms, texture->xGL, texture->yGL, texture->xscale, texture->yscale);
		if (texture->angle != 0) {
			setBaseUniformTransformAngle((BaseShaderUniforms*)texture->renderer->baseShader->uniforms, texture->angle);
		}
		setBaseUniformClipOffset((BaseShaderUniforms*)texture->renderer->baseShader->uniforms, texture->clipOffset.x, texture->clipOffset.y);
        setBaseUniformColorAlphaMod((BaseShaderUniforms*)texture->renderer->baseShader->uniforms, texture->colorMod, texture->alphaMod);
    }
    applyShaderUniforms(texture->renderer->baseShader);

    glBindVertexArray(texture->VAOid);
        glBindTexture(GL_TEXTURE_2D, texture->id);

            glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}

void invertPixels(SDL_Surface* srf)
{
    size_t rowSize = srf->w * srf->format->BytesPerPixel;
    Uint8* row = malloc(rowSize);
    Uint8* pixels = srf->pixels;
    int i;
    for(i = 0; i < srf->h/2; i++)
    {
        memcpy(row, pixels + i*srf->pitch, rowSize);
        memcpy(pixels + i*srf->pitch, pixels + (srf->h - i -1)*srf->pitch, rowSize);
        memcpy(pixels + (srf->h - i -1)*srf->pitch, row, rowSize);
    }
    free(row);
}

void destroyTexture(Texture* texture)
{
    if(texture == NULL) return;
    if(texture->VBOid != 0) glDeleteBuffers(1, &texture->VBOid);
    if(texture->VAOid != 0) glDeleteVertexArrays(1, &texture->VAOid);
    if(texture->id != 0) glDeleteTextures(1, &texture->id);
    free(texture);
}

int getTextureBPP(Texture* tex)
{
    if(tex->internalFormat == GL_RGBA8) return 4;
    return 3;
}

