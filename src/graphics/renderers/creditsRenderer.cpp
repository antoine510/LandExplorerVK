#include "creditsRenderer.h"
#include "../graphics.h"

CreditsRenderer* createCreditsRenderer()
{
    CreditsRenderer* creditsRenderer = (CreditsRenderer*)calloc(1, sizeof(CreditsRenderer));

    return creditsRenderer;
}

void initCreditsRenderer(Credits* credits, Graphics* gfx)
{
    SDL_Color textColor = {255, 255, 255, 255};
    int i;
    for(i = 0; i < credits->lineCount; i++)
    {
        TTF_Font* font = TTF_OpenFont("txt.ttf", credits->lineSize[i]);
		gfx->creditsRenderer->lineTextures[i] = new Sprite(credits->lines[i], textColor, font);
		gfx->creditsRenderer->lineTextures[i]->setOrigin(0.5f, 0).setScreenOrigin(0.5f, 0);
        TTF_CloseFont(font);
    }

    TTF_Font* font = TTF_OpenFont("txt.ttf", credits->endLineSize);
    gfx->creditsRenderer->endLineTexture = new Sprite(credits->endLine, textColor, font);
	gfx->creditsRenderer->endLineTexture->setOrigin(0.5f, 0.5f).setScreenOrigin(0.5f, 0.5f).setPosition(0, 0);
    TTF_CloseFont(font);
}

void renderCredits(Graphics* gfx, Credits* credits)
{
    renderBackground(gfx->bgRenderer, gfx, BG_WIN);

    int y = (int)credits->baseY;
    int i;
    for(i = 0; i < MAX_CREDIT_LINES; i++)
    {
        if(gfx->creditsRenderer->lineTextures[i] != NULL)
        {
			gfx->creditsRenderer->lineTextures[i]->setPosition(0, y);
            gfx->creditsRenderer->lineTextures[i]->draw(gfx->cmdBuf);
            y -= gfx->creditsRenderer->lineTextures[i]->getExtent().height + 5;
        }
        else break;
    }

    if(y < 0)
    {
        gfx->creditsRenderer->endLineTexture->draw(gfx->cmdBuf);
    }
}

void destroyCreditsRenderer(CreditsRenderer* creditsRenderer)
{
    int i;
    for(i = 0; i < MAX_CREDIT_LINES; i++)
    {
        delete creditsRenderer->lineTextures[i];
    }
    delete creditsRenderer->endLineTexture;
    free(creditsRenderer);
}

