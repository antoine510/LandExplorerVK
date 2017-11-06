#include "textureSet.h"

TextureSet* createTextureSet(xmlNodePtr texSetNode, const char* path)
{
    TextureSet* texSet = (TextureSet*)calloc(1, sizeof(TextureSet));

    int i;
    for(i = 0; i < MAX_ANIMATION_USER; i++) texSet->curAnim[i] = -1;
    texSet->curUser = 0;
    texSet->animated = 0;

    Texture* stateTex;
    xmlNodePtr texture = texSetNode->children;
    while(texture->type == XML_TEXT_NODE) texture = texture->next;

    int textureCount = 0, animCount = 0;
    while(texture)
    {
		std::string filename(path);
		filename += asStringl(texture, "filename");
        stateTex = createTextureFromFile(renderer, filename.c_str());

        if(checkName(texture, "state")) {
            setTexture(texSet, textureCount, stateTex);
            textureCount++;
        } else if (checkName(texture, "animation")) {
            int frameWidth = asIntl(texture, "frameWidth");
            setTextureClipSize(stateTex, frameWidth, stateTex->h);

            setTexture(texSet, textureCount, stateTex);

            Animation* anim = initAnimation(textureCount, stateTex->w, frameWidth, asIntl(texture, "delay"), asBooll(texture, "loop"));
            setAnimation(texSet, anim, animCount);

            animCount++;
            textureCount++;
        } else if (checkName(texture, "mapIcon")) {
			setTextureOriginRatio(stateTex, 0.5f, 0.5f);
            setMapIcon(texSet, stateTex);
        }

        do texture = texture->next; while(texture && texture->type == XML_TEXT_NODE);
    }

    return texSet;
}

void setTexture(TextureSet* texSet, unsigned int textureID, Sprite* tex)
{
    if(textureID >= MAX_TEXTURE_COUNT)  //TODO : SDL_assert
        printf("Error while assigning texture, please increase MAX_TEXTURE_COUNT");
    texSet->textures[textureID] = tex;
}

void setAnimation(TextureSet* texSet, Animation* animation, unsigned int id)
{
    if(id >= MAX_ANIMATION_COUNT) printf("Error while assigning id, please increase MAX_ANIMATION_COUNT");
    texSet->animations[id] = animation;
    texSet->animated = 1;
}

void setMapIcon(TextureSet* texSet, Sprite* mapIcon)
{
    texSet->mapIcon = mapIcon;
}

void setTexSetColorMod(TextureSet* texSet, SDL_Color* color)
{
    int i;
    for(i = 0; i < MAX_TEXTURE_COUNT; i++)
    {
        if(texSet->textures[i] != NULL) texSet->textures[i]->setColorMod(*color);
    }
}

Texture* getTexture(TextureSet* texSet, int textureID)
{
    if(!texSet->animated) return texSet->textures[textureID];

    if(textureID >= ANIMATION_OFFSET)    //textureID
    {
        int animID = textureID - ANIMATION_OFFSET;
        if(texSet->curAnim[texSet->curUser] != animID && texSet->curAnim[texSet->curUser] != -1)
            reset(texSet->animations[texSet->curAnim[texSet->curUser]], texSet->curUser);
        texSet->curAnim[texSet->curUser] = animID;
        texSet->curUser++;
        setTextureClip(texSet->textures[texSet->animations[animID]->textureID],
                       getFrameClip(texSet->animations[animID], texSet->curUser -1), 0);
        return texSet->textures[texSet->animations[animID]->textureID];
    }
    else        //Animation
    {
        if(texSet->curAnim[texSet->curUser] != -1)
        {
            reset(texSet->animations[texSet->curAnim[texSet->curUser]], texSet->curUser);
            texSet->curAnim[texSet->curUser] = -1;
        }
        texSet->curUser++;
        return texSet->textures[textureID];
    }
}

void texSetFrameReset(TextureSet* texSet)
{
    texSet->curUser = 0;
}

void destroyTextureSet(TextureSet* texSet)
{
    int i;
    for(i = 0; i < MAX_TEXTURE_COUNT; i++)
    {
        if(texSet->textures[i] != NULL) destroyTexture(texSet->textures[i]);
    }
    for(i = 0; i < MAX_ANIMATION_COUNT; i++)
    {
        if(texSet->animations[i] != NULL) destroyAnimation(texSet->animations[i]);
    }
    destroyTexture(texSet->mapIcon);
    free(texSet);
}
