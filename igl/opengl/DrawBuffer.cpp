//
// Created by hedi1 on 24/01/2022.
//

#include "DrawBuffer.h"
#include "gl.h"
#include <iostream>


igl::opengl::DrawBuffer::DrawBuffer()
{
    frameBuffer = 0;
    renderBuffer = 0;
    depthBuffer = 0;
    depthStencilBuffer = 0;
    isStencil = false;
}

igl::opengl::DrawBuffer::DrawBuffer(int width, int height, bool stencil, unsigned int texId )
{
    bool fboUsed = true;
    CreateColorBufferAttachment(width, height, texId);

    if (stencil)
        CreateStencilBufferAttachment(width, height,texId + 1);
    else
        CreateDepthBufferAttachment(width, height, texId + 1);
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        fboUsed = false;
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }
    UnBind();
    isStencil = stencil;
}

void igl::opengl::DrawBuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    //glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    //if(isStencil)
    //	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    //else
    //	glBindRenderbuffer(GL_RENDERBUFFER, depthStencilBuffer);
    //glBindRenderbuffer(GL_RENDERBUFFER, stencilBuffer);

}


void igl::opengl::DrawBuffer::UnBind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //glBindRenderbuffer(GL_RENDERBUFFER, 0);
    //glBindRenderbuffer(GL_RENDERBUFFER, GL_DEPTH_BUFFER);
}




void igl::opengl::DrawBuffer::CreateColorBufferAttachment(int width, int height, unsigned int texId)
{
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, 0);
    //glGenRenderbuffers(1, &renderBuffer);
    //glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height);
    //glBindRenderbuffer(GL_RENDERBUFFER, 0);
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentNum , GL_RENDERBUFFER, renderBuffer);
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //glBindRenderbuffer(GL_RENDERBUFFER, 0);
}



void igl::opengl::DrawBuffer::CreateDepthBufferAttachment(int width, int height, unsigned int texId)
{
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texId, 0);

    //glGenRenderbuffers(1, &depthBuffer);
    //glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    //glBindRenderbuffer(GL_RENDERBUFFER, 0);
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
}

void igl::opengl::DrawBuffer::CreateStencilBufferAttachment(int width, int height, unsigned int texId)
{
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, texId, 0);
    //glGenRenderbuffers(1, &depthStencilBuffer);
    //glBindRenderbuffer(GL_RENDERBUFFER, depthStencilBuffer);
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    //glBindRenderbuffer(GL_RENDERBUFFER, 0);
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthStencilBuffer);


}


void igl::opengl::DrawBuffer::resize(int width, int height, unsigned int texId)
{
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    //glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGBA8, width, height);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_RENDERBUFFER, renderBuffer);

    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    //glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

igl::opengl::DrawBuffer::~DrawBuffer(void)
{
    glDeleteFramebuffers(1, &frameBuffer);
    glDeleteFramebuffers(1, &renderBuffer);
    if(isStencil)
        glDeleteRenderbuffers(1, &depthStencilBuffer);
    else
        glDeleteRenderbuffers(1, &depthBuffer);
}

void igl::opengl::DrawBuffer::clearFrameBuffers(Eigen::Vector4i viewport, Eigen::Vector4f background_color)
{
    // The glScissor call ensures we only clear this core's buffers,
    // (in case the user wants different background colors in each viewport.)
    glScissor((int) viewport(0), (int) viewport(1), (int) viewport(2), (int) viewport(3));
    glEnable(GL_SCISSOR_TEST);
    glClearColor(background_color[0],
                 background_color[1],
                 background_color[2],
                 background_color[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
}