//
// Created by hedi1 on 24/01/2022.
//

#ifndef ENIGEREWORK_DRAWBUFFER_H
#define ENIGEREWORK_DRAWBUFFER_H
#include <Eigen/Core>
namespace igl
{
    namespace opengl {
        class DrawBuffer {
            //NOTE: old enigen
            unsigned int frameBuffer;
            unsigned int renderBuffer;
            unsigned int depthBuffer;
            unsigned int depthStencilBuffer;
            bool isStencil;

            void CreateDepthBufferAttachment(int width, int height, unsigned int texId);

            void CreateColorBufferAttachment(int width, int height, unsigned int texId);

            void CreateStencilBufferAttachment(int width, int height, unsigned int texId);


        public:
            //NOTE: old enigen
            enum {
                COLOR,
                DEPTH,
                STENCIL,
                BACK,
                FRONT,
                NONE,
            };

            DrawBuffer();

            DrawBuffer(int width, int height, bool stencil, unsigned int texId);

            void resize(int width, int height, unsigned int texId);

            void Bind();

            void UnBind();

            ~DrawBuffer(void);

            //NOTE: needed for the new one
            void clearFrameBuffers(Eigen::Vector4i viewport, Eigen::Vector4f background_color);

        };
    }
}

#endif //ENIGEREWORK_DRAWBUFFER_H
