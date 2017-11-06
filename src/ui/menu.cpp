#include "menu.h"
#include "utility/xmlTools.h"
#include "utility/mathUtility.h"
#include "sound.h"
#include "graphics/displayInfo.h"
#include "graphics/graphics.h"

static void initSubMenu(SubMenu* subMenu, xmlNodePtr subMenuNode, Vec2 alignment);
static void createKeyLabels(SubMenu* submenu, KeyStates* keystates);
static void updateKeyLabels(SubMenu* submenu, KeyStates* keystates, KeyType type, int index);

Menu* initMenu(KeyStates* keystates) {
	Menu* menu = (Menu*)calloc(1, sizeof(Menu));

	menu->currentSubMenu = 0;
	menu->stateChanged = true;

	xmlDocPtr menuDoc = parseXML("ui/menu.xml");
	xmlNodePtr menuNode = xmlDocGetRootElement(menuDoc);

	//Gets alignment and converts it from [0..1] to [-1..1]
	Vec2 alignment = Vec2{ asFloatl(menuNode, "Walign")*2.0f - 1.0f, asFloatl(menuNode, "Halign")*2.0f - 1.0f };

	xmlNodePtr subMenuNode = menuNode->children;
	while(subMenuNode->type == XML_TEXT_NODE) subMenuNode = subMenuNode->next;
	while(subMenuNode) {
		if(checkName(subMenuNode, "SubMenu")) {
			int subMenuID = asIntl(subMenuNode, "id");

			initSubMenu(&menu->subMenus[subMenuID], subMenuNode, alignment);
		}

		do subMenuNode = subMenuNode->next; while(subMenuNode && subMenuNode->type == XML_TEXT_NODE);
	}

	xmlFreeDoc(menuDoc);

	createKeyLabels(&menu->subMenus[SUBMENU_KEY_BINDINGS], keystates);

	return menu;
}

void initSubMenu(SubMenu* subMenu, xmlNodePtr subMenuNode, Vec2 alignment) {
	subMenu->name = asStringl(subMenuNode, "name");
	subMenu->layout = layout_create();
	subMenu->buttonCount = 0;
	subMenu->labelCount = 0;

	xmlNodePtr button = subMenuNode->children;
	while(button->type == XML_TEXT_NODE) button = button->next;
	while(button) {
		if(checkName(button, "Button")) {
			int buttonID = asIntl(button, "id");
			SDL_Rect buttonRect = getRectl(button);		buttonRect.x += (int)(myDisplayMode.w / 2 * alignment.x);	buttonRect.y += (int)(myDisplayMode.h / 2 * alignment.y);
			char* buttonName = asStringl(button, "name");
			subMenu->buttons[buttonID] = createButton(buttonRect, buttonName);
			free(buttonName);

			layout_addElement(subMenu->layout, centeredRect(&buttonRect, false, true), buttonID);
			subMenu->buttonCount++;
		} else if(checkName(button, "Slider")) {
			int sliderID = asIntl(button, "id");
			SDL_Point pos = getPosl(button); pos.x += (int)(myDisplayMode.w / 2 * alignment.x);	pos.y += (int)(myDisplayMode.h / 2 * alignment.y);
			subMenu->sliders[sliderID] = slider_create(asStringl(button, "name"), pos, asIntl(button, "value"),
													   asIntl(button, "min"), asIntl(button, "max"));
			SDL_Rect minusRect = slider_getMinusRect(subMenu->sliders[sliderID]);
			SDL_Rect plusRect = slider_getPlusRect(subMenu->sliders[sliderID]);
			subMenu->buttons[2 * sliderID] = createButton(minusRect, "-");
			subMenu->buttons[2 * sliderID + 1] = createButton(plusRect, "+");
			layout_addElement(subMenu->layout, centeredRect(&minusRect, false, true), subMenu->buttonCount);
			layout_addElement(subMenu->layout, centeredRect(&plusRect, false, true), subMenu->buttonCount + 1);
			subMenu->buttonCount += 2;
		} else if(checkName(button, "Label")) {
			int labelID = asIntl(button, "id");
			SDL_Point pos = getPosl(button); pos.x += (int)(myDisplayMode.w / 2 * alignment.x);	pos.y += (int)(myDisplayMode.h / 2 * alignment.y);
			subMenu->labels[labelID] = label_create(asStringl(button, "text"), pos);
			subMenu->labelCount++;
		}

		do button = button->next; while(button && button->type == XML_TEXT_NODE);
	}

	subMenu->buttons[0]->state = BUTTON_SELECTED;   //Select the first button (there is at least one button)
	subMenu->selection = 0;
}

ModeUpdateResult updateMenu(Menu* menu, KeyStates* keyStates, Graphics* gfx) {
	int x, y;
	SDL_GetMouseState(&x, &y);
	x -= myDisplayMode.w / 2;
	y = myDisplayMode.h / 2 - y;

	SubMenu* subMenu = &menu->subMenus[menu->currentSubMenu];
	if(x != menu->oldMousePos.x || y != menu->oldMousePos.y) {
		int mouseSelection = layout_getElement(subMenu->layout, x, y);
		if(mouseSelection != -1 && mouseSelection != subMenu->selection) {
			subMenu->buttons[subMenu->selection]->state = BUTTON_UNSELECTED;
			subMenu->selection = mouseSelection;
			subMenu->buttons[subMenu->selection]->state = BUTTON_SELECTED;
			menu->stateChanged = true;
			soundstack_addSound(SOUND_MENU_TICK);
		}
		menu->oldMousePos.x = x; menu->oldMousePos.y = y;
	}

	if(isKeyHeld(keyStates, key_down) && subMenu->selection < subMenu->buttonCount - 1) {
		subMenu->buttons[subMenu->selection]->state = BUTTON_UNSELECTED;
		subMenu->selection++;
		subMenu->buttons[subMenu->selection]->state = BUTTON_SELECTED;
		menu->stateChanged = true;
		soundstack_addSound(SOUND_MENU_TICK);
	} else if(isKeyHeld(keyStates, key_up) && subMenu->selection > 0) {
		subMenu->buttons[subMenu->selection]->state = BUTTON_UNSELECTED;
		subMenu->selection--;
		subMenu->buttons[subMenu->selection]->state = BUTTON_SELECTED;
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
				toggleDisplayFullscreen(gfx);
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
			soundstack_changeMasterVolume((float)subMenu->sliders[0]->value / 100.0f);
			soundstack_changeMusicVolume((float)subMenu->sliders[1]->value / 100.0f);
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
	submenu->labels[0] = label_create(keystates_getKeyName(keystates, key_quit), SDL_Point{ submenu->buttons[0]->rect.x + 110, submenu->buttons[0]->rect.y });
	submenu->labels[1] = label_create(keystates_getKeyName(keystates, key_left), SDL_Point{ submenu->buttons[1]->rect.x + 110, submenu->buttons[1]->rect.y });
	submenu->labels[2] = label_create(keystates_getKeyName(keystates, key_right), SDL_Point{ submenu->buttons[2]->rect.x + 110, submenu->buttons[2]->rect.y });
	submenu->labels[3] = label_create(keystates_getKeyName(keystates, key_up), SDL_Point{ submenu->buttons[3]->rect.x + 110, submenu->buttons[3]->rect.y });
	submenu->labels[4] = label_create(keystates_getKeyName(keystates, key_down), SDL_Point{ submenu->buttons[4]->rect.x + 110, submenu->buttons[4]->rect.y });
	submenu->labels[5] = label_create(keystates_getKeyName(keystates, key_jump), SDL_Point{ submenu->buttons[5]->rect.x + 160, submenu->buttons[5]->rect.y });
	submenu->labels[6] = label_create(keystates_getKeyName(keystates, key_select), SDL_Point{ submenu->buttons[6]->rect.x + 160, submenu->buttons[6]->rect.y });
	submenu->labels[7] = label_create(keystates_getKeyName(keystates, key_map), SDL_Point{ submenu->buttons[7]->rect.x + 160, submenu->buttons[7]->rect.y });
	submenu->labels[8] = label_create(keystates_getKeyName(keystates, key_editor), SDL_Point{ submenu->buttons[8]->rect.x + 160, submenu->buttons[8]->rect.y });
	submenu->labels[9] = label_create(keystates_getKeyName(keystates, key_inventory), SDL_Point{ submenu->buttons[9]->rect.x + 160, submenu->buttons[9]->rect.y });
	submenu->labelCount = 10;
}

void updateKeyLabels(SubMenu* submenu, KeyStates* keystates, KeyType type, int index) {
	submenu->labels[index].text = keystates_getKeyName(keystates, type);
}

void destroyMenu(Menu* menu) {
	int i, j;
	for(i = 0; i < SUBMENU_COUNT; i++) {
		layout_destroy(menu->subMenus[i].layout);
		free(menu->subMenus[i].name);
		for(j = 0; j < menu->subMenus[i].buttonCount; j++) {
			destroyButton(menu->subMenus[i].buttons[j]);
		}
		for(j = 0; j < menu->subMenus[i].labelCount; j++) {
			label_destroy(&menu->subMenus[i].labels[j]);
		}
		for(j = 0; j < MAX_MENU_ELEMENT_COUNT; j++) {
			if(menu->subMenus[i].sliders[j] != NULL) slider_destroy(menu->subMenus[i].sliders[j]);
		}
	}
	free(menu);
}
