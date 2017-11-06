#pragma once

#include "utility/vector.h"
#include "vulkan/Image.h"
#include "vulkan/DescriptorSet.h"
#include "vulkan/Pipeline.h"
#include "displayInfo.h"

typedef struct _TTF_Font TTF_Font;
struct SDL_Color;
class Swapchain;

class Sprite : public StagedImage {
public:
	Sprite(const std::string& path);
	Sprite(const std::string& text, const SDL_Color& color, TTF_Font* font);
	Sprite(unsigned int width, unsigned int height);
	~Sprite();

	void setScale(float scale) {
		setScreenSize(unsigned int(_wreal * scale), unsigned int(_hreal * scale));
	}
	void setFullscreen();

	void setPosition(int posX, int posY) {
		_pushConsts.pos.x = -1.0f + (float(posX) - float(_wreal) * origin.x) / (myDisplayMode.w >> 1);
		_pushConsts.pos.y = -1.0f + (float(posY) - float(_hreal) * origin.y) / (myDisplayMode.h >> 1);
	}

	void setOrigin(float rX, float rY) { origin.x = rX; origin.y = rY; }

	void setClipSize(unsigned int wc, unsigned int hc) {
		_pushConsts.texCoords.z = float(wc) / _w;
		_pushConsts.texCoords.w = float(hc) / _h;
		setScreenSize(wc, hc);
	}
	void setClip(unsigned int ox, unsigned int oy) {
		_pushConsts.texCoords.x = ox * _pushConsts.texCoords.z;
		_pushConsts.texCoords.y = oy * _pushConsts.texCoords.w;
	}

	void setColorMod(const SDL_Color& colorMod) {
		_pushConsts.colorAlphaMod.r = colorMod.r / 255.0f;
		_pushConsts.colorAlphaMod.g = colorMod.g / 255.0f;
		_pushConsts.colorAlphaMod.b = colorMod.b / 255.0f;
	}
	void setAlphaMod(float alphaMod) { _pushConsts.colorAlphaMod.a = alphaMod; }

	void draw(vk::CommandBuffer& cmdBuf) {
		_descSet->writeBinding(DescriptorSetBinding(0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment, getImageInfo()));
		cmdBuf.pushConstants<PushConstants>(_pipeline->getLayout(), vk::ShaderStageFlagBits::eAllGraphics, 0, _pushConsts);
		cmdBuf.draw(6, 1, 0, 0);
	}

	static void setupSpriteRendering(const Swapchain& swapchain);
	static void startSpriteRendering(const vk::CommandBuffer& cmdBuf);
	static void teardownSpriteRendering();

private:
	static VertexBuffer* _vBuf;
	static DescriptorSet* _descSet;
	static Pipeline* _pipeline;
	static std::vector<Shader> _shaders;

	struct PushConstants {
		glm::mat2 sizeRot;
		glm::vec4 texCoords;
		glm::vec4 colorAlphaMod;
		glm::vec2 pos;
	};
	void setScreenSize(unsigned int w, unsigned int h) {
		_pushConsts.sizeRot[0][0] = float(w) / (myDisplayMode.w >> 1);
		_pushConsts.sizeRot[1][1] = float(h) / (myDisplayMode.h >> 1);
		_wreal = w; _hreal = h;
	}

	unsigned int _wreal, _hreal;
	glm::vec2 origin;	// Normalized origin vector
	PushConstants _pushConsts;
};

/*typedef struct Texture {
	int w, h;
	float xscale, yscale;
	float angle;
	SDL_Color* colorMod;
	float alphaMod;
	int clipW, clipH;
	Vec2 clipOffset;

	float xGL, yGL, wGL, hGL;
} Texture;

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

int getTextureBPP(Texture* tex);*/
