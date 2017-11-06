#include "textureGroup.h"
#include "../displayInfo.h"

TextureGroup* createTextureGroup(OGLRenderer* renderer, int wElement, int hElement, int wCount, int hCount, int wTarget, int hTarget,
                                 int modeCount, const char* shaderName, ShaderType type)
{
    TextureGroup* texGroup = malloc(sizeof(TextureGroup));

    texGroup->wCount = wCount; texGroup->hCount = hCount;
    texGroup->wRatio = 1.0f/wCount; texGroup->hRatio =  1.0f/hCount;
    texGroup->wTarget = wTarget; texGroup->hTarget = hTarget;
    texGroup->wGL = (float)wElement / myDisplayMode->w *2; texGroup->hGL = (float)hElement / myDisplayMode->h *2;
    texGroup->modeCount = modeCount;
    texGroup->renderer = renderer;
    texGroup->textures = calloc(modeCount, sizeof(Texture*));
    texGroup->textureIndices = calloc(wTarget * hTarget, sizeof(int));
    texGroup->type = type;

    float vertices[] = {0.0f, 0.0f,				texGroup->wGL, 0.0f,
												texGroup->wGL, texGroup->hGL,
						0.0f, 0.0f,
						0.0f, texGroup->hGL,	texGroup->wGL, texGroup->hGL};
    float texCoords[] = {0, texGroup->hRatio,	texGroup->wRatio, texGroup->hRatio,
												texGroup->wRatio, 0,
						0, texGroup->hRatio,
						0, 0,					texGroup->wRatio, 0};
	/*float* vertices = malloc(wTarget * hTarget * 12 * sizeof(float));
	float* texCoords = malloc(wTarget * hTarget * 12 * sizeof(float));
    int x, y, i;
    for(x = 0; x < wTarget; x++)
    {
        for(y = 0; y < hTarget; y++)
        {
            for(i = 0; i < 6; i++)
            {
                vertices[(x*hTarget+y) * 12 +i*2] = sourceVertices[i*2] + x * texGroup->wGL;
                vertices[(x*hTarget+y) * 12 +i*2 +1] = sourceVertices[i*2 +1] - y * texGroup->hGL;
            }
            memcpy(texCoords + (x*hTarget+y)*12, sourceTexCoords, 12*sizeof(float));
        }
    }*/

    createVBOandVAO(&texGroup->VBOid, &texGroup->VAOid, vertices, texCoords, 12);
	//free(vertices);
	//free(texCoords);

    char vertName[MAX_FILENAME_SIZE] = "graphics/shaders/", fragName[MAX_FILENAME_SIZE];
    strcat(vertName, shaderName);
    strcpy(fragName, vertName);
    strcat(vertName, ".vert"); strcat(fragName, ".frag");

    texGroup->groupShader = createShaderFromSource(vertName, fragName);
    if(!loadShader(texGroup->groupShader)) return NULL;

    if(type == shader_chunck)
        setUniformsData(texGroup->groupShader, (void*)createChunckShaderUniforms(texGroup->groupShader, texGroup->wGL, texGroup->hGL), setChunckShaderUniforms);
    else if(type == shader_base)
        setUniformsData(texGroup->groupShader, (void*)createBaseShaderUniforms(texGroup->groupShader), applyBaseShaderUniforms);

    return texGroup;
}

void setTextureGroupColorMod(TextureGroup* texGroup, SDL_Color* colorMod)
{
    texGroup->colorMod = colorMod;
    setChunckUniformColorMod(texGroup->groupShader->uniforms, colorMod);
}

void setTextureGroupTextureBank(TextureGroup* texGroup, Texture* tex, int mode)
{
    texGroup->textures[mode] = tex;
}

void setTextureInGroup(TextureGroup* texGroup, int textureIndex, int x, int y)
{
    texGroup->textureIndices[x*texGroup->hTarget + y] = textureIndex;
}

void clearTextureGroup(TextureGroup* texGroup)
{
    int i;
    for(i = 0; i < texGroup->wTarget * texGroup->hTarget; i++)
    {
        texGroup->textureIndices[i] = -1;
    }
}

// Position x, y represents the upper left corner of the Target
void drawTextureGroup(TextureGroup* texGroup, int x, int y, int mode)
{
	float xGL = (float)x / myDisplayMode->w * 2, yGL = (float)y / myDisplayMode->h * 2;

    glUseProgram(texGroup->groupShader->programID);
    glBindVertexArray(texGroup->VAOid);

        if(texGroup->type == shader_base)
        {
            setBaseUniformColorAlphaMod((BaseShaderUniforms*)texGroup->groupShader->uniforms, texGroup->colorMod, 1.0f);
        }

        applyShaderUniforms(texGroup->groupShader);

        glBindTexture(GL_TEXTURE_2D, texGroup->textures[mode]->id);

        for(int xC = 0; xC < texGroup->wTarget; xC++)
        {
            for(int yC = 0; yC < texGroup->hTarget; yC++)
            {
                int textureIndex = texGroup->textureIndices[xC * texGroup->hTarget + yC];
                if(textureIndex != -1)
                {
                    applyChunckUniformClipOffset((ChunckShaderUniforms*)texGroup->groupShader->uniforms,
                                                texGroup->wRatio*(textureIndex / texGroup->hCount),
                                                texGroup->hRatio*(textureIndex % texGroup->hCount));
					applyChunckUniformPosition((ChunckShaderUniforms*)texGroup->groupShader->uniforms, xGL + texGroup->wGL * xC, -yGL - (yC+1) * texGroup->hGL);
                    glDrawArrays(GL_TRIANGLES, 0, 6);
                }
            }
        }
        glBindTexture(GL_TEXTURE_2D, 0);

    glBindVertexArray(0);
    startFrameTexturesRendering(texGroup->renderer);
}

void destroyTextureGroup(TextureGroup* texGroup)
{
    if(texGroup->type == shader_chunck)
        destroyChunckShaderUniforms((ChunckShaderUniforms*)texGroup->groupShader->uniforms);
    destroyShader(texGroup->groupShader);
    if(texGroup->VBOid != 0) glDeleteBuffers(1, &texGroup->VBOid);
    if(texGroup->VAOid != 0) glDeleteVertexArrays(1, &texGroup->VAOid);
    free(texGroup->textureIndices);
    free(texGroup->textures);
    free(texGroup);
}
