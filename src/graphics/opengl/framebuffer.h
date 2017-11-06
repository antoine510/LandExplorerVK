#pragma once

#include "oglCommon.h"
#include "texture.h"

typedef struct FrameBuffer {
    GLuint id;
    Texture* colorBuffer;
} FrameBuffer;

bool fb_init(FrameBuffer* frameBuffer, Texture* renderingTexture);

void fb_startRendering(FrameBuffer* frameBuffer);
void fb_endRendering();

void fb_destroy(FrameBuffer* frameBuffer);

