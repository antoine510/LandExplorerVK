#include "renderer.h"

OGLRenderer* createOGLRenderer(SDL_Window* window)
{
    OGLRenderer* oglRenderer = malloc(sizeof(OGLRenderer));

    oglRenderer->baseShader = createShaderFromSource("graphics/shaders/texture.vert", "graphics/shaders/texture.frag");
    if(!loadShader(oglRenderer->baseShader)) return NULL;
    setUniformsData(oglRenderer->baseShader, (void*)createBaseShaderUniforms(oglRenderer->baseShader), applyBaseShaderUniforms);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return oglRenderer;
}

void startFrameTexturesRendering(OGLRenderer* renderer)
{
    glUseProgram(renderer->baseShader->programID);
}

void clearScreen(SDL_Color* color)
{
    glClearColor((float)color->r / 255, (float)color->g / 255, (float)color->b / 255, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
}

void endFrameTexturesRendering()
{
    glUseProgram(0);
}

void createVBOandVAO(GLuint* VBOid, GLuint* VAOid, float* vertices, float* texCoords, int vertexCount)
{
    size_t vertexDataSize = vertexCount * sizeof(float);
    glGenBuffers(1, VBOid);
    glBindBuffer(GL_ARRAY_BUFFER, *VBOid);

        //Allocation memoire
        glBufferData(GL_ARRAY_BUFFER, 2*vertexDataSize, 0, GL_STATIC_DRAW);

        glBufferSubData(GL_ARRAY_BUFFER, 0, vertexDataSize, vertices);
        glBufferSubData(GL_ARRAY_BUFFER, vertexDataSize, vertexDataSize, texCoords);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, VAOid);
    glBindVertexArray(*VAOid);

        glBindBuffer(GL_ARRAY_BUFFER, *VBOid);

            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (char*)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (char*)vertexDataSize);
            glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void updateVBO(GLuint VBOid, float* vertices, float* texCoords, int vertexCount)
{
    glBindBuffer(GL_ARRAY_BUFFER, VBOid);

        char* VBOadress = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

        if(vertices != NULL) memcpy(VBOadress, vertices, vertexCount*sizeof(float));
        VBOadress += vertexCount*sizeof(float);

        if(texCoords != NULL) memcpy(VBOadress, texCoords, vertexCount*sizeof(float));

        glUnmapBuffer(GL_ARRAY_BUFFER);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void destroyOGLRenderer(OGLRenderer* renderer)
{
	destroyBaseShaderUniforms((BaseShaderUniforms*)renderer->baseShader->uniforms);
    destroyShader(renderer->baseShader);
    free(renderer);
}
