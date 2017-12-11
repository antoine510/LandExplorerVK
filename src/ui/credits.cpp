#include "credits.h"
#include "utility/xmlTools.h"
#include "graphics/displayInfo.h"

static void loadCredits(Credits* credits);

Credits* createCredits()
{
    Credits* credits = (Credits*)calloc(1, sizeof(Credits));

    credits->lineCount = 0;
    credits->baseY = float(myDisplayMode.h + 10);

    loadCredits(credits);
    return credits;
}

void loadCredits(Credits* credits)
{
    xmlDocPtr creditsDoc = parseXML("ui/credits.xml");
	xmlNodePtr line = xmlDocGetRootElement(creditsDoc)->xmlChildrenNode;

    while(line->type == XML_TEXT_NODE) line = line->next;
    while (line)
    {
        if(checkName(line, "Line"))
        {
            credits->lines[credits->lineCount] = getNodeContent(line);
            credits->lineSize[credits->lineCount] = checkAttributeExists(line, "size") ? asIntl(line, "size") : 30;

            credits->lineCount++;
        } else if (checkName(line, "EndLine")) {
            credits->endLine = getNodeContent(line);
            credits->endLineSize = asIntl(line, "size");
        }

        do line = line->next; while(line && line->type == XML_TEXT_NODE);
    }

	xmlFreeDoc(creditsDoc);
}

void updateCredits(Credits* credits)
{
    credits->baseY -= (40.0f / FPS);
}

void destroyCredits(Credits* credits)
{
    for(int i = 0; i < credits->lineCount; i++) free(credits->lines[i]);

    free(credits);
}

