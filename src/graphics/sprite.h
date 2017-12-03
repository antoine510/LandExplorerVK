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
	Sprite(SDL_Surface* srf) : StagedImage(srf), _realSize(_w, _h) { setScreenSize(_realSize); }
	Sprite(const std::string& text, const SDL_Color& color, TTF_Font* font);
	Sprite(unsigned int width, unsigned int height);
	Sprite(const Sprite& other) = delete;

	~Sprite() { _descSet.erase(); }

	Sprite& setScale(float scale) { setScreenSize(_realSize * scale); return *this; }
	Sprite& setScale(float xScale, float yScale) { setScreenSize(_realSize * Vec2(xScale, yScale)); return *this; }
	Sprite& setFullscreen() { setPosition(0, 0); return *this; }

	Sprite& setPosition(int posX, int posY) {
		_pushConsts.pos = Vec2(-1, -1) + 2.f * _screenOrigin + (Vec2(posX, posY) - _realSize * _origin) / Vec2(myDisplayMode.w >> 1, myDisplayMode.h >> 1);
		return *this;
	}
	Sprite& setLayer(unsigned int layer) { _pushConsts.layer = layer; }

	Sprite& setOrigin(float rX, float rY) { _origin = Vec2(rX, rY); return *this; }
	Sprite& setScreenOrigin(float rX, float rY) { _screenOrigin = Vec2(rX, rY); return *this; }

	Sprite& setClipSize(unsigned int wc, unsigned int hc) {
		_realSize = Vec2(wc, hc);
		_pushConsts.texCoords.z = float(wc) / _w;
		_pushConsts.texCoords.w = float(hc) / _h;
		setScreenSize(_realSize);
		return *this;
	}
	Sprite& setClip(unsigned int ox, unsigned int oy) {
		_pushConsts.texCoords.x = ox * _pushConsts.texCoords.z;
		_pushConsts.texCoords.y = oy * _pushConsts.texCoords.w;
		return *this;
	}

	Sprite& setColorMod(const SDL_Color& colorMod) {
		_pushConsts.colorAlphaMod.r = colorMod.r / 255.0f;
		_pushConsts.colorAlphaMod.g = colorMod.g / 255.0f;
		_pushConsts.colorAlphaMod.b = colorMod.b / 255.0f;
		return *this;
	}
	Sprite& setAlphaMod(float alphaMod) { _pushConsts.colorAlphaMod.a = alphaMod; return *this; }

	Sprite& fillColor(SDL_Color color) {
		std::vector<uint32_t> BGRAColors(_wPitch * _h, uint32_t(color.b) + (uint32_t(color.g) << 8) + (uint32_t(color.r) << 16) + (uint32_t(color.a) << 24));
		_staging->update(BGRAColors.data());
		stageImage();
		return *this;
	}

	void draw(vk::CommandBuffer& cmdBuf) {
		cmdBuf.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, _pipeline->getLayout(), 0u, (const vk::DescriptorSet&)_descSet, nullptr);
		cmdBuf.pushConstants<PushConstants>(_pipeline->getLayout(), vk::ShaderStageFlagBits::eAllGraphics, 0, _pushConsts);
		cmdBuf.draw(6, 1, 0, 0);
	}

	static void setupSpriteRendering(const Swapchain& swapchain);
	static void startSpriteRendering(const vk::CommandBuffer& cmdBuf);
	static void teardownSpriteRendering();

private:
	static constexpr int maxSprites = 100;

	static VertexBuffer* _vBuf;
	static vk::DescriptorPool _descPool;
	static vk::DescriptorSetLayout _descLayout;
	static std::vector<DescriptorSet> _descSets;
	static Pipeline* _pipeline;
	static std::vector<Shader> _shaders;

	struct PushConstants {
		glm::mat2 sizeRot;
		glm::vec4 texCoords;
		glm::vec4 colorAlphaMod;
		glm::vec2 pos;
		uint32_t layer;
	};

	DescriptorSet& getDescSet() {
		auto res = std::find_if(_descSets.begin(), _descSets.end(), [](const DescriptorSet& set)->bool { return !set.isWritten(); });
		SDL_assert(res != _descSets.end());
		res->writeBinding(DescriptorSetBinding(0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment, getImageInfo()));
		return *res;
	}

	void setScreenSize(Vec2 size) {
		_pushConsts.sizeRot[0][0] = size.x / (myDisplayMode.w >> 1);
		_pushConsts.sizeRot[1][1] = size.y / (myDisplayMode.h >> 1);
	}

	Vec2 _realSize;
	Vec2 _origin;		// Normalized origin vector
	Vec2 _screenOrigin;	// Normalized screen origin vector
	PushConstants _pushConsts;
	DescriptorSet& _descSet = getDescSet();
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

