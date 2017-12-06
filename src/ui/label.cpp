#include "label.h"

Label label_create(const std::string& name, SDL_Point pos, Vec2 screenOrigin) {
	Label label;
	label.text = name;
	label.pos = pos;
	label.screenOrigin = screenOrigin;

	return label;
}

void label_destroy(Label* label) {}

