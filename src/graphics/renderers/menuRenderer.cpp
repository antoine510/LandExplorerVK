#include "menuRenderer.h"
#include "../graphics.h"

#define TITLE_FONT_SIZE 80
#define BUTTON_FONT_SIZE 35
#define SELECTED_BUTTON_FONT_SIZE 45

static void updateMenuTextures(MenuRenderer* menuRenderer, Graphics* gfx, Menu* menu);
static void destroyMenuTextures(MenuRenderer* menuRenderer);
static SliderTextures createSliderTextures(Graphics* gfx, Slider* slider, TTF_Font* font, SDL_Color color);

void menuRendererLoadTextures(MenuRenderer* menuRenderer, xmlNodePtr mainNode) {
	xmlNodePtr texture = mainNode->children;
	while(texture->type == XML_TEXT_NODE) texture = texture->next;

	while(texture) {
		std::string filename("textures/");
		filename += asStringl(mainNode, "path");
		filename += asStringl(texture, "path");
		if(checkName(texture, "outline")) {
			menuRenderer->outline = new Sprite(filename);
			menuRenderer->outline->setPosition(10, 10);
		}
		do texture = texture->next; while(texture && texture->type == XML_TEXT_NODE);
	}
}

void initMenuRenderer(MenuRenderer* menuRenderer) {
	menuRenderer->titleFont = TTF_OpenFont("txt.ttf", TITLE_FONT_SIZE);
	menuRenderer->buttonFont = TTF_OpenFont("txt.ttf", BUTTON_FONT_SIZE);
	menuRenderer->selectedButtonFont = TTF_OpenFont("txt.ttf", SELECTED_BUTTON_FONT_SIZE);

	menuRenderer->titleColor = SDL_Color{249, 214, 91, 255};
	menuRenderer->buttonColor = SDL_Color{240, 240, 240, 255};
	menuRenderer->selectedButtonColor = SDL_Color{255, 255, 180, 255};
	menuRenderer->labelColor = SDL_Color{255, 255, 50, 255};
}

void renderMenu(Graphics* gfx, Menu* menu) {
	if(menu->stateChanged) {
		updateMenuTextures(&gfx->menuRenderer, gfx, menu);
		menu->stateChanged = false;
	}

	renderBackground(gfx->bgRenderer, gfx, BG_MENU);

	gfx->menuRenderer.outline->draw(gfx->cmdBuf);
	gfx->menuRenderer.titleTexture->draw(gfx->cmdBuf);

	for(const auto& buttonTex : gfx->menuRenderer.buttonTextures) buttonTex.draw(gfx->cmdBuf);
	for(const auto& labelTex : gfx->menuRenderer.labelTextures) labelTex.draw(gfx->cmdBuf);
	for(const auto& sliderTex : gfx->menuRenderer.sliderTextures) {
		sliderTex.nameTexture->draw(gfx->cmdBuf);
		sliderTex.valueTexture->draw(gfx->cmdBuf);
	}
}

void updateMenuTextures(MenuRenderer* menuRenderer, Graphics* gfx, Menu* menu) {
	SubMenu* subMenu = &menu->subMenus[menu->currentSubMenu];

	destroyMenuTextures(menuRenderer);

	menuRenderer->titleTexture = new Sprite(subMenu->name, menuRenderer->titleColor, menuRenderer->titleFont);
	menuRenderer->titleTexture->setPosition(25, 20);

	int i;
	for(i = 0; i < subMenu->buttonCount; i++) {
		TTF_Font* buttonFont = (i == subMenu->selection) ? menuRenderer->selectedButtonFont : menuRenderer->buttonFont;
		SDL_Color buttonColor = (i == subMenu->selection) ? menuRenderer->selectedButtonColor : menuRenderer->buttonColor;
		menuRenderer->buttonTextures.emplace_back(subMenu->buttons[i]->name, buttonColor, buttonFont);
		menuRenderer->buttonTextures.back().setScreenOrigin(subMenu->buttons[i]->screenOrigin).setOrigin(0, 0.5f).setPosition(subMenu->buttons[i]->rect.x, subMenu->buttons[i]->rect.y);
	}
	for(i = 0; i < subMenu->labelCount; i++) {
		menuRenderer->labelTextures.emplace_back(subMenu->labels[i].text, menuRenderer->labelColor, menuRenderer->buttonFont);
		menuRenderer->labelTextures.back().setScreenOrigin(subMenu->labels[i].screenOrigin).setOrigin(0, 0.5f).setPosition(subMenu->labels[i].pos.x, subMenu->labels[i].pos.y);
	}
	for(i = 0; i < MAX_MENU_ELEMENT_COUNT; i++) {
		if(subMenu->sliders[i] != NULL) {
			menuRenderer->sliderTextures.push_back(createSliderTextures(gfx, subMenu->sliders[i], menuRenderer->buttonFont, menuRenderer->buttonColor));
		}
	}
}

SliderTextures createSliderTextures(Graphics* gfx, Slider* slider, TTF_Font* font, SDL_Color color) {
	SliderTextures tex;
	tex.nameTexture = new Sprite(slider->name, color, font);
	tex.nameTexture->setScreenOrigin(slider->screenOrigin).setOrigin(0, 0.5f).setPosition(slider->pos.x, slider->pos.y);

	tex.valueTexture = new Sprite(std::to_string(slider->value), color, font);
	tex.valueTexture->setScreenOrigin(slider->screenOrigin).setOrigin(0, 0.5f).setPosition(slider->pos.x + 370, slider->pos.y);

	return tex;
}

void destroyMenuTextures(MenuRenderer* menuRenderer) {
	delete menuRenderer->titleTexture;
	menuRenderer->titleTexture = nullptr;

	menuRenderer->buttonTextures.clear();
	menuRenderer->labelTextures.clear();
	for(const auto& sliderTex : menuRenderer->sliderTextures) {
		delete sliderTex.nameTexture;
		delete sliderTex.valueTexture;
	}
	menuRenderer->sliderTextures.clear();
}

void destroyMenuRenderer(MenuRenderer* menuRenderer) {
	destroyMenuTextures(menuRenderer);
	delete menuRenderer->outline;

	TTF_CloseFont(menuRenderer->titleFont);
	TTF_CloseFont(menuRenderer->buttonFont);
	TTF_CloseFont(menuRenderer->selectedButtonFont);
}

