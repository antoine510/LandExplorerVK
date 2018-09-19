#include "luaScript.h"
#include <cctype>
#include <sstream>

LuaScript::LuaScript(const std::string& filename, std::unordered_map<std::string, int>&& constants) : L(luaL_newstate()) {
	for(const auto& pair : constants) {
		set(std::string(pair.first), pair.second);
	}
	if(luaL_loadfile(L, filename.c_str()) || lua_pcall(L, 0, 0, 0)) {
		throw LuaException(filename + " not loaded: " + lua_tostring(L, -1));
	}
}

LuaScript::StackScope::StackScope(lua_State* state, const ScopeDescriptor& scopeDescriptor) : L(state), savedLevel(lua_gettop(L)) {
	const auto& scope = scopeDescriptor._scope;
	if(scope.size() == 0) return;
	int level = savedLevel;
	if(level == 0) {
		lua_getglobal(L, scope[0].c_str());
		if(scope.size() > 1 && lua_isnil(L, -1)) throw LuaException("Global " + scope[0] + " is not defined");
		level++;
	}
	for(; level < savedLevel + scope.size(); ++level) {
		if(std::isdigit(static_cast<unsigned char>(scope[level - savedLevel][0]))) {	// variable name starts with a digit: index
			int index = 0;
			std::istringstream(scope[level - savedLevel]) >> index;
			lua_geti(L, -1, index);
		} else {
			lua_getfield(L, -1, scope[level - savedLevel].c_str());
		}
		if(level < savedLevel + scope.size() - 1 && lua_isnil(L, -1)) throw LuaException(scope[level - savedLevel] + " is not defined");
	}
}

#include "graphics/sprite.h"

template <typename T>
T pickIfValid(T value, T invalid, T def) { return value == invalid ? def : value; }

template <>
Sprite* LuaScript::_get() {
	Sprite* res = new Sprite(get<std::string>("path"));
	if(has("clipSize")) res->setClipSize(pickIfValid(get<int>({"clipSize", "w"}), -1, (int)res->getExtent().width),
										 pickIfValid(get<int>({"clipSize", "h"}), -1, (int)res->getExtent().height));
	if(has("screenOrigin")) res->setScreenOrigin(get<float>({"screenOrigin", "x"}), get<float>({"screenOrigin", "y"}));
	if(has("origin")) res->setOrigin(get<float>({"origin", "x"}), get<float>({"origin", "y"}));
	if(has("position")) res->setPosition(get<int>({"position", "x"}), get<int>({"position", "y"}));
	if(has("scale")) res->setScale(get<float>("scale"));
	if(hasTrue("linearSampling")) res->setSampling(true);
	if(hasTrue("fullscreen")) res->setFullscreen();
	return res;
}

template <>
SDL_Point LuaScript::_get() {
	return {get<int>("X"), get<int>("Y")};
}

template <>
SDL_Rect LuaScript::_get() {
	return {get<int>("X"), get<int>("Y"), get<int>("W"), get<int>("H")};
}

template <>
Vec2 LuaScript::_get() {
	return Vec2(get<float>("X"), get<float>("Y"));
}
