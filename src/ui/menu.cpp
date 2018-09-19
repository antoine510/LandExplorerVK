#include "menu.h"
#include "luaScript.h"
#include "utility/mathUtility.h"
#include "sound.h"
#include "graphics/displayInfo.h"
#include "graphics/graphics.h"

static void initSubMenu(SubMenu& subMenu, LuaScript& script, Vec2 screenOrigin);
static void createKeyLabels(SubMenu* submenu, KeyStates* keystates);
static void updateKeyLabels(SubMenu* submenu, KeyStates* keystates, KeyType type, int index);

Menu* initMenu(KeyStates* keystates) {
	Menu* menu = new Menu;

	menu->currentSubMenu = 0;
	menu->stateChanged = true;

	LuaScript script("ui/menu.lua");

	Vec2 screenOrigin(script.get<float>("Walign"), script.get<float>("Halign"));


	auto scope(script.getScope("submenus"));
	int submenuCount = script.getLength();
	if(submenuCount != SUBMENU_COUNT) throw std::runtime_error("Error while assigning submenus: wrong submenu count");
	for(int i = 0; i < submenuCount; ++i) {
		auto scope(script.getScope(i + 1));	// Lua arrays are indexed from 1
		initSubMenu(menu->subMenus[i], script, screenOrigin);
	}

	createKeyLabels(&menu->subMenus[SUBMENU_KEY_BINDINGS], keystates);

	return menu;
}

void initSubMenu(SubMenu& subMenu, LuaScript& script, Vec2 screenOrigin) {
	subMenu.name = script.get<std::string>("name");
	subMenu.layout = layout_create();

	auto scope(script.getScope("elements"));
	int eltCount = script.getLength();
	for(int i = 0; i < eltCount; ++i) {
		auto scope(script.getScope(i + 1));
		auto type = script.get<std::string>("type");
		if(type == "button") {
			subMenu.buttons.emplace_back(createButton(script.get<SDL_Rect>(), script.get<std::string>("name"), screenOrigin));
			layout_addElement(subMenu.layout, rectOriginRatio(script.get<SDL_Rect>(), 0, 0.5f), (int)subMenu.buttons.size() - 1, screenOrigin);
		} else if(type == "slider") {
			subMenu.sliders.emplace_back(slider_create(script.get<std::string>("name"), script.get<SDL_Point>(),
													   script.get<int>("value"), script.get<int>("min"), script.get<int>("max"), screenOrigin));
			SDL_Rect minusRect = slider_getMinusRect(subMenu.sliders.back());
			SDL_Rect plusRect = slider_getPlusRect(subMenu.sliders.back());
			subMenu.buttons.emplace_back(createButton(minusRect, "-", screenOrigin));
			subMenu.buttons.emplace_back(createButton(plusRect, "+", screenOrigin));
			layout_addElement(subMenu.layout, rectOriginRatio(minusRect, 0, 0.5f), (int)subMenu.buttons.size() - 2, screenOrigin);
			layout_addElement(subMenu.layout, rectOriginRatio(plusRect, 0, 0.5f), (int)subMenu.buttons.size() - 1, screenOrigin);
		} else if(type == "label") {
			subMenu.labels.emplace_back(label_create(script.get<std::string>("text"), script.get<SDL_Point>(), screenOrigin));
		}
	}

	subMenu.buttons[0].state = BUTTON_SELECTED;   //Select the first button (there is at least one button)
	subMenu.selection = 0;
}

ModeUpdateResult updateMenu(Menu* menu, KeyStates* keyStates, Graphics* gfx) {
	int x, y;
	SDL_GetMouseState(&x, &y);

	SubMenu* subMenu = &menu->subMenus[menu->currentSubMenu];
	if(x != menu->oldMousePos.x || y != menu->oldMousePos.y) {
		int mouseSelection = layout_getElement(subMenu->layout, x, y);
		if(mouseSelection != -1 && mouseSelection != subMenu->selection) {
			subMenu->buttons[subMenu->selection].state = BUTTON_UNSELECTED;
			subMenu->selection = mouseSelection;
			subMenu->buttons[subMenu->selection].state = BUTTON_SELECTED;
			menu->stateChanged = true;
			soundstack_addSound(SOUND_MENU_TICK);
		}
		menu->oldMousePos.x = x; menu->oldMousePos.y = y;
	}

	if(isKeyHeld(keyStates, key_down) && subMenu->selection < subMenu->buttons.size() - 1) {
		subMenu->buttons[subMenu->selection].state = BUTTON_UNSELECTED;
		subMenu->selection++;
		subMenu->buttons[subMenu->selection].state = BUTTON_SELECTED;
		menu->stateChanged = true;
		soundstack_addSound(SOUND_MENU_TICK);
	} else if(isKeyHeld(keyStates, key_up) && subMenu->selection > 0) {
		subMenu->buttons[subMenu->selection].state = BUTTON_UNSELECTED;
		subMenu->selection--;
		subMenu->buttons[subMenu->selection].state = BUTTON_SELECTED;
		menu->stateChanged = true;
		soundstack_addSound(SOUND_MENU_TICK);
	}

	ModeUpdateResult muResult = MURESULT_MENU;
	if(isKeyHeld(keyStates, key_select) || (isMouseLeftDown() && layout_getElement(subMenu->layout, x, y) != -1)) {
		switch(menu->currentSubMenu) {
		case SUBMENU_MAIN_MENU:
			switch(subMenu->selection) {
			case 0:
				muResult = MURESULT_LEVEL_LOAD;
				break;
			case 1:
				menu->currentSubMenu = SUBMENU_OPTIONS;
				soundstack_addSound(SOUND_MENU_OPEN);
				break;
			case 2:
				menu->currentSubMenu = SUBMENU_INSTRUCTIONS;
				soundstack_addSound(SOUND_MENU_OPEN);
				break;
			case 3:
				muResult = MURESULT_QUIT;
				break;
			}
			break;
		case SUBMENU_OPTIONS:
			switch(subMenu->selection) {
			case 0:
				menu->currentSubMenu = SUBMENU_GRAPHIC_OPTIONS;
				soundstack_addSound(SOUND_MENU_OPEN);
				break;
			case 1:
				menu->currentSubMenu = SUBMENU_AUDIO_OPTIONS;
				soundstack_addSound(SOUND_MENU_OPEN);
				break;
			case 2:
				menu->currentSubMenu = SUBMENU_KEY_BINDINGS;
				soundstack_addSound(SOUND_MENU_OPEN);
				break;
			case 3:
				menu->currentSubMenu = SUBMENU_MAIN_MENU;
				soundstack_addSound(SOUND_MENU_CLOSE);
				break;
			}
			break;
		case SUBMENU_GRAPHIC_OPTIONS:
			switch(subMenu->selection) {
			case 0:
				setDisplaySize(gfx, 1920, 1080);
				break;
			case 1:
				setDisplaySize(gfx, 1024, 768);
				break;
			case 2:
				setDisplaySize(gfx, 800, 600);
				break;
			case 3:
				setDisplaySize(gfx, 0, 0, true);
				break;
			case 4:
				menu->currentSubMenu = SUBMENU_OPTIONS;
				soundstack_addSound(SOUND_MENU_CLOSE);
				break;
			}
			break;
		case SUBMENU_AUDIO_OPTIONS:
			switch(subMenu->selection) {
			case 0:
				slider_changeValue(subMenu->sliders[0], -10);
				break;
			case 1:
				slider_changeValue(subMenu->sliders[0], 10);
				break;
			case 2:
				slider_changeValue(subMenu->sliders[1], -10);
				break;
			case 3:
				slider_changeValue(subMenu->sliders[1], 10);
				break;
			case 4:
				menu->currentSubMenu = SUBMENU_OPTIONS;
				soundstack_addSound(SOUND_MENU_CLOSE);
				break;
			}
			soundstack_changeMasterVolume((float)subMenu->sliders[0].value / 100.0f);
			soundstack_changeMusicVolume((float)subMenu->sliders[1].value / 100.0f);
			break;
		case SUBMENU_KEY_BINDINGS:
			switch(subMenu->selection) {
			case 0: keystates_setKey(keyStates, key_quit); updateKeyLabels(subMenu, keyStates, key_quit, subMenu->selection); break;
			case 1: keystates_setKey(keyStates, key_left); updateKeyLabels(subMenu, keyStates, key_left, subMenu->selection); break;
			case 2: keystates_setKey(keyStates, key_right); updateKeyLabels(subMenu, keyStates, key_right, subMenu->selection); break;
			case 3: keystates_setKey(keyStates, key_up); updateKeyLabels(subMenu, keyStates, key_up, subMenu->selection); break;
			case 4: keystates_setKey(keyStates, key_down); updateKeyLabels(subMenu, keyStates, key_down, subMenu->selection); break;
			case 5: keystates_setKey(keyStates, key_jump); updateKeyLabels(subMenu, keyStates, key_jump, subMenu->selection); break;
			case 6: keystates_setKey(keyStates, key_select); updateKeyLabels(subMenu, keyStates, key_select, subMenu->selection); break;
			case 7: keystates_setKey(keyStates, key_map); updateKeyLabels(subMenu, keyStates, key_map, subMenu->selection); break;
			case 8: keystates_setKey(keyStates, key_editor); updateKeyLabels(subMenu, keyStates, key_editor, subMenu->selection); break;
			case 9: keystates_setKey(keyStates, key_inventory); updateKeyLabels(subMenu, keyStates, key_inventory, subMenu->selection); break;
			case 10:
				menu->currentSubMenu = SUBMENU_OPTIONS;
				soundstack_addSound(SOUND_MENU_CLOSE);
				break;
			}
			break;
		case SUBMENU_INSTRUCTIONS:
			switch(subMenu->selection) {
			case 0:
				menu->currentSubMenu = SUBMENU_MAIN_MENU;
				soundstack_addSound(SOUND_MENU_CLOSE);
				break;
			}
			break;
		}
		menu->stateChanged = true;
	}
	return muResult;
}

void createKeyLabels(SubMenu* submenu, KeyStates* keystates) {
	submenu->labels.emplace_back(label_create(keystates_getKeyName(keystates, key_quit), SDL_Point{ submenu->buttons[0].rect.x + 110, submenu->buttons[0].rect.y }, submenu->buttons[0].screenOrigin));
	submenu->labels.emplace_back(label_create(keystates_getKeyName(keystates, key_left), SDL_Point{ submenu->buttons[1].rect.x + 110, submenu->buttons[1].rect.y }, submenu->buttons[1].screenOrigin));
	submenu->labels.emplace_back(label_create(keystates_getKeyName(keystates, key_right), SDL_Point{ submenu->buttons[2].rect.x + 110, submenu->buttons[2].rect.y }, submenu->buttons[2].screenOrigin));
	submenu->labels.emplace_back(label_create(keystates_getKeyName(keystates, key_up), SDL_Point{ submenu->buttons[3].rect.x + 110, submenu->buttons[3].rect.y }, submenu->buttons[3].screenOrigin));
	submenu->labels.emplace_back(label_create(keystates_getKeyName(keystates, key_down), SDL_Point{ submenu->buttons[4].rect.x + 110, submenu->buttons[4].rect.y }, submenu->buttons[4].screenOrigin));
	submenu->labels.emplace_back(label_create(keystates_getKeyName(keystates, key_jump), SDL_Point{ submenu->buttons[5].rect.x + 160, submenu->buttons[5].rect.y }, submenu->buttons[5].screenOrigin));
	submenu->labels.emplace_back(label_create(keystates_getKeyName(keystates, key_select), SDL_Point{ submenu->buttons[6].rect.x + 160, submenu->buttons[6].rect.y }, submenu->buttons[6].screenOrigin));
	submenu->labels.emplace_back(label_create(keystates_getKeyName(keystates, key_map), SDL_Point{ submenu->buttons[7].rect.x + 160, submenu->buttons[7].rect.y }, submenu->buttons[7].screenOrigin));
	submenu->labels.emplace_back(label_create(keystates_getKeyName(keystates, key_editor), SDL_Point{ submenu->buttons[8].rect.x + 160, submenu->buttons[8].rect.y }, submenu->buttons[8].screenOrigin));
	submenu->labels.emplace_back(label_create(keystates_getKeyName(keystates, key_inventory), SDL_Point{ submenu->buttons[9].rect.x + 160, submenu->buttons[9].rect.y }, submenu->buttons[9].screenOrigin));
}

void updateKeyLabels(SubMenu* submenu, KeyStates* keystates, KeyType type, int index) {
	submenu->labels[index].text = keystates_getKeyName(keystates, type);
}

void destroyMenu(Menu* menu) {
	int i;
	for(i = 0; i < SUBMENU_COUNT; i++) {
		layout_destroy(menu->subMenus[i].layout);
	}
	delete menu;
}
