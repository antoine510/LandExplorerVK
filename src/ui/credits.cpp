#include "credits.h"
#include "luaScript.h"
#include "graphics/displayInfo.h"

static void loadCredits(Credits* credits);

Credits* createCredits()
{
	Credits* credits = new Credits;

    credits->lineCount = 0;
    credits->baseY = float(myDisplayMode.h + 10);

    loadCredits(credits);
    return credits;
}

void loadCredits(Credits* credits)
{
	LuaScript script("ui/credits.lua");

	{
		auto scope(script.getScope("creditLines"));
		credits->lineCount = script.getLength();
		for(int i = 0; i < credits->lineCount; ++i) {
			auto scope(script.getScope(i + 1));	// Lua arrays are indexed from 1
			credits->lines[i] = script.get<std::string>("text");
			credits->lineSize[i] = script.has("size") ? script.get<int>("size") : 30;
		}
	}
	credits->endLine = script.get<std::string>({"endLine", "text"});
	credits->endLineSize = script.get<int>({"endLine", "size"});
}

void updateCredits(Credits* credits)
{
    credits->baseY -= (40.0f / FPS);
}

void destroyCredits(Credits* credits)
{
    delete credits;
}

