#pragma once

#include "common.h"
#include "utility/vector.h"
#include "vulkan/Image.h"
#include "vulkan/DescriptorSet.h"
#include "vulkan/Pipeline.h"
#include "displayInfo.h"
#include "utility/mathUtility.h"

#include <SDL_ttf.h>

typedef struct _TTF_Font TTF_Font;
struct SDL_Color;
class Swapchain;

struct SpriteRenderer {
	SpriteRenderer(const Swapchain& swapchain);
	~SpriteRenderer();

	void bind(const vk::CommandBuffer& cmdBuf) {
		cmdBuf.bindPipeline(vk::PipelineBindPoint::eGraphics, *_pipeline);
		cmdBuf.bindVertexBuffers(0, (vk::Buffer)*_vBuf, (vk::DeviceSize)0u);
	}

	DescriptorSet& getDescSet() {
		auto res = std::find_if(_descSets.begin(), _descSets.end(), [](const DescriptorSet& set)->bool { return !set.isWritten(); });
		SDL_assert(res != _descSets.end());
		return *res;
	}

	static constexpr int maxSprites = 100;

	VertexBuffer* _vBuf;
	vk::DescriptorPool _descPool;
	vk::DescriptorSetLayout _descLayout;
	std::vector<DescriptorSet> _descSets;
	Pipeline* _pipeline;
	std::vector<Shader> _shaders;
};
extern SpriteRenderer* spriteRenderer;

class Sprite : public SampledImage {
public:
	Sprite(const std::string& path) : SampledImage(path) {}
	Sprite(SDL_Surface* srf) : SampledImage(srf) {}
	Sprite(const std::string& text, const SDL_Color& color, TTF_Font* font) : SampledImage(TTF_RenderText_Blended(font, text.c_str(), color)) {}
	Sprite(unsigned int width, unsigned int height) : SampledImage(width, height) {}
	Sprite(const Sprite& other) = delete;

	~Sprite() { _descSet.erase(); }

	Sprite& setScale(float scale) { setScreenSize(_realSize * scale); return *this; }
	Sprite& setScale(float xScale, float yScale) { setScreenSize(_realSize * Vec2(xScale, yScale)); return *this; }
	Sprite& setFullscreen() { setPosition(0, 0); setScreenSize(Vec2(myDisplayMode.w, myDisplayMode.h)); return *this; }

	Sprite& setPosition(int posX, int posY) { setPosition(Vec2(posX, posY)); return *this; }
	Sprite& setPositionBloc(float posX, float posY) { setPosition(Vec2(posX * BLOC_SIZE, posY * BLOC_SIZE)); return *this; }
	Sprite& setPosition(Vec2 pos) {
		_pushConsts.pos = Vec2(-1, -1) + 2.f * _screenOrigin + (pos - _realSize * _origin) / Vec2(myDisplayMode.w >> 1, myDisplayMode.h >> 1);
		return *this;
	}
	Sprite& setLayer(unsigned int layer) { _pushConsts.layer = layer; }

	Sprite& setOrigin(float rX, float rY) { _origin = Vec2(rX, rY); return *this; }
	Sprite& setScreenOrigin(float rX, float rY) { _screenOrigin = Vec2(rX, rY); return *this; }
	Sprite& setScreenOrigin(Vec2 o) { _screenOrigin = o; return *this; }

	Sprite& setClipSize(unsigned int wc, unsigned int hc) {
		_realSize = Vec2(wc, hc);
		_pushConsts.texCoords.z = float(wc) / _extent.width;
		_pushConsts.texCoords.w = float(hc) / _extent.height;
		setScreenSize(_realSize);
		return *this;
	}
	Sprite& setClip(unsigned int ox, unsigned int oy) {
		_pushConsts.texCoords.x = ox * _pushConsts.texCoords.z;
		_pushConsts.texCoords.y = oy * _pushConsts.texCoords.w;
		return *this;
	}

	Sprite& setColorMod(SDL_Color colorMod) { _pushConsts.colorAlphaMod = colorToVec(colorMod); return *this; }
	Sprite& setColorMod(Vec4 colorMod) { _pushConsts.colorAlphaMod = colorMod; return *this; }
	Sprite& setAlphaMod(float alphaMod) { _pushConsts.colorAlphaMod.a = alphaMod; return *this; }

	Sprite& fillColor(SDL_Color color) {
		std::vector<uint32_t> BGRAColors(_wPitch * _extent.height, uint32_t(color.b) + (uint32_t(color.g) << 8) + (uint32_t(color.r) << 16) + (uint32_t(color.a) << 24));
		uploadPixels(BGRAColors.data());
		return *this;
	}

	void draw(vk::CommandBuffer& cmdBuf) {
		cmdBuf.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, spriteRenderer->_pipeline->getLayout(), 0u, (const vk::DescriptorSet&)_descSet, nullptr);
		cmdBuf.pushConstants<PushConstants>(spriteRenderer->_pipeline->getLayout(), vk::ShaderStageFlagBits::eAllGraphics, 0, _pushConsts);
		cmdBuf.draw(6, 1, 0, 0);
	}

private:
	struct PushConstants {
		PushConstants(Vec2 texSize) : sizeRot(texSize.x / (myDisplayMode.w >> 1), 0, 0, texSize.y / (myDisplayMode.h >> 1)) {}

		glm::mat2 sizeRot;
		glm::vec4 texCoords = glm::vec4(0, 0, 1, 1);
		glm::vec4 colorAlphaMod = glm::vec4(1, 1, 1, 1);
		glm::vec2 pos = glm::vec2(-1, -1);
		uint32_t layer = 2;
	};

	DescriptorSet& genDescSet() {
		auto& res = spriteRenderer->getDescSet();
		res.writeBinding(DescriptorSetBinding(0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment, getImageInfo()));
		return res;
	}

	void setScreenSize(Vec2 size) {
		_pushConsts.sizeRot[0][0] = size.x / (myDisplayMode.w >> 1);
		_pushConsts.sizeRot[1][1] = size.y / (myDisplayMode.h >> 1);
	}

	Vec2 _realSize = Vec2(_extent.width, _extent.height);
	Vec2 _origin;		// Normalized origin vector
	Vec2 _screenOrigin;	// Normalized screen origin vector
	PushConstants _pushConsts = PushConstants(_realSize);
	DescriptorSet& _descSet = genDescSet();
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

