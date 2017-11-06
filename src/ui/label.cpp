#include "label.h"

Label label_create(const std::string& name, SDL_Point pos) {
	Label label;
	label.text = name;
	label.pos = pos;

	return label;
}

void label_destroy(Label* label) {}

