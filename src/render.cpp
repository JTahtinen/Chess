#include "render.h"
#include <jadel.h>
#include "graphics.h"
#include "math.h"
#include "game.h"
#include "defs.h"

struct RenderableDimensions
{
    jadel::Rectf rect;
    jadel::Mat3 transform;
};

struct RectRenderable
{
    RenderableDimensions dimensions;
    uint32 color;
};

struct TextureRenderable
{
    RenderableDimensions dimensions;
    const Texture *texture;
};

enum
{
    RENDERABLE_RECT,
    RENDERABLE_TEXTURE
};

jadel::Vector<RectRenderable> rectRenderableQueue;
jadel::Vector<TextureRenderable> textureRenderableQueue;
jadel::Vector<uint32> renderableTypeQueue;

jadel::Mat3 projMatrix;

void renderSubmitRectRenderable(jadel::Rectf dim, uint32 color, jadel::Mat3 transform)
{
    RectRenderable renderable;
    renderable.dimensions.rect = dim;
    renderable.dimensions.transform = transform;
    renderable.color = color;
    rectRenderableQueue.push(renderable);
    renderableTypeQueue.push(RENDERABLE_RECT);
}

void renderSubmitRectRenderable(jadel::Vec2 start, jadel::Vec2 end, uint32 color, jadel::Mat3 transform)
{
    renderSubmitRectRenderable(jadel::Rectf(start, end), color, transform);
}

void renderSubmitRectRenderable(float xStart, float yStart, float xEnd, float yEnd, uint32 color, jadel::Mat3 transform)
{
    renderSubmitRectRenderable(jadel::Vec2(xStart, yStart), jadel::Vec2(xEnd, yEnd), color, transform);
}

void renderSubmitTextureRenderable(jadel::Rectf dim, const Texture *texture, jadel::Mat3 transform)
{
    TextureRenderable renderable;
    renderable.dimensions.rect = dim;
    renderable.dimensions.transform = transform;
    renderable.texture = texture;
    textureRenderableQueue.push(renderable);
    renderableTypeQueue.push(RENDERABLE_TEXTURE);
}

void renderSubmitTextureRenderable(jadel::Vec2 start, jadel::Vec2 end, const Texture* texture, jadel::Mat3 transform)
{
    renderSubmitTextureRenderable(jadel::Rectf(start, end), texture, transform);
}

void renderSubmitTextureRenderable(float xStart, float yStart, float xEnd, float yEnd, const Texture *texture, jadel::Mat3 transform)
{
    renderSubmitTextureRenderable(jadel::Vec2(xStart, yStart), jadel::Vec2(xEnd, yEnd), texture, transform);
}



void renderSubmitWorldRect(jadel::Rectf rect, jadel::Vec2 pos, uint32 color)
{
    jadel::Mat3 mvp = getTranslationMatrix(pos) * projMatrix;
    renderSubmitRectRenderable(rect, color, mvp);
}

void renderSubmitWorldTexture(jadel::Rectf rect, jadel::Vec2 pos, const Texture *texture)
{
    jadel::Mat3 mvp = getTranslationMatrix(pos) * projMatrix;
    renderSubmitTextureRenderable(rect, texture, mvp);
}

void renderSubmitScreenRect(jadel::Rectf rect, jadel::Vec2 pos, uint32 color)
{
    jadel::Mat3 mvp = projMatrix * getTranslationMatrix(pos);
    renderSubmitRectRenderable(rect, color, mvp);
}

void renderSubmitScreenTexture(jadel::Rectf rect, jadel::Vec2 pos, const Texture *texture)
{
    jadel::Mat3 mvp = projMatrix * getTranslationMatrix(pos);
    renderSubmitTextureRenderable(rect, texture, mvp);
}

void renderClear()
{
    graphicsClear();
}

void renderInit()
{
    projMatrix = jadel::Mat3(1.0f / aspect, 0, 0,
                             0, 1.0f, 0,
                             0, 0, 1.0f);

    jadel::vectorInit(50, &rectRenderableQueue);
    jadel::vectorInit(50, &textureRenderableQueue);
    jadel::vectorInit(100, &renderableTypeQueue);
}

void renderFlush()
{
    int numRectsRendered = 0;
    int numTexturesRendered = 0;
    for (uint32 type : renderableTypeQueue)
    {
        switch(type)
        {
            case RENDERABLE_RECT:
            {
                RectRenderable renderable = rectRenderableQueue[numRectsRendered++];
                graphicsDrawRect(renderable.dimensions.rect, renderable.color, renderable.dimensions.transform);
                break;
            }
            case RENDERABLE_TEXTURE:
            {
                TextureRenderable renderable = textureRenderableQueue[numTexturesRendered++];
                graphicsDrawTexture(renderable.dimensions.rect, renderable.texture, renderable.dimensions.transform);
                break;
            }
            default:
            {
                ERRMSG("Invalid renderable type\n");
                break;
            }
        }
    }
    rectRenderableQueue.clear();
    textureRenderableQueue.clear();
    renderableTypeQueue.clear();
    
}