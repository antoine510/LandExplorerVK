#include "framebuffer.h"
#include "../displayInfo.h"

bool fb_init(FrameBuffer* frameBuffer, Texture* renderingTexture)
{
    glGenFramebuffers(1, &frameBuffer->id);

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->id);
        frameBuffer->colorBuffer = renderingTexture;
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBuffer->colorBuffer->id, 0);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            printf("Couldn't initialize FBO\n");
            glDeleteFramebuffers(1, &frameBuffer->id);

            return false;
        }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

void fb_startRendering(FrameBuffer* frameBuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->id);
    glViewport(0, 0, frameBuffer->colorBuffer->w, frameBuffer->colorBuffer->h);
}

void fb_endRendering()
{
    glViewport(0, 0, myDisplayMode->w, myDisplayMode->h);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void fb_destroy(FrameBuffer* frameBuffer)
{
    glDeleteFramebuffers(1, &frameBuffer->id);
}

