#pragma once

#include "oglCommon.h"
#include "shader.h"
#include "baseShader.h"

typedef struct OGLRenderer {
    Shader* baseShader;
} OGLRenderer;

OGLRenderer* createOGLRenderer(SDL_Window* window);

void startFrameTexturesRendering(OGLRenderer* renderer);
void clearScreen(SDL_Color* color);
void endFrameTexturesRendering();

void createVBOandVAO(GLuint* VBOid, GLuint* VAOid, float* vertices, float* texCoords, int vertexCount);
void updateVBO(GLuint VBOid, float* vertices, float* texCoords, int vertexCount);

void destroyOGLRenderer(OGLRenderer* renderer);

