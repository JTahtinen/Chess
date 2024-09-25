#include "graphics.h"
#include "defs.h"
#include "util.h"

float aspect;
static jadel::Window window;
static int screenWidth;
static int screenHeight;
jadel::Surface winSurface;
static jadel::Graphics graphics;

bool bilinearFilterMode = false;

jadel::Surface errorSurface;
Texture errorTexture;

void initErrorSurface()
{
    jadel::graphicsCreateSurface(4, 4, &errorSurface);
    graphics.pushTargetSurface(&errorSurface);
    graphics.setClearColor(0xff00ff00);
    graphics.clearTargetSurface();
    graphics.drawRectFast(1, 3, 1, 3, jadel::Color(1.0f, 1.0f, 0.0f, 1.0f));
    graphics.drawPixelFast(0, 0, 0xff0000ff);
    graphics.drawPixelFast(3, 3, 0xffffff00);
    graphics.popTargetSurface();
    graphics.setClearColor(0);

    errorTexture = graphicsCreateTexture(jadel::Vec2(0, 0), jadel::Vec2(1.0f, 1.0f), &errorSurface);
}

void graphicsInit(int clientWidth, int clientHeight)
{
    graphics.init();
    jadel::windowCreate(&window, "Chess", clientWidth, clientHeight);
    jadel::graphicsCreateSurface(clientWidth, clientHeight, &winSurface);
    graphics.pushTargetSurface(&winSurface);
    graphics.setClearColor(0xff000000);
    graphics.clearTargetSurface();
    initErrorSurface();
    screenWidth = clientWidth;
    screenHeight = clientHeight;
    aspect = (float)clientWidth / (float)clientHeight;
    
}

void graphicsUpdate()
{
    if (jadel::inputIsKeyTyped(jadel::KEY_B))
    {
        graphicsSetFilterMode(!bilinearFilterMode);
    }
    jadel::windowUpdate(&window, &winSurface);
}

void graphicsClear()
{
    graphics.clearTargetSurface();
}

void graphicsDrawRect(jadel::Vec2 start, jadel::Vec2 end, uint32 color, jadel::Mat3 transform)
{
    jadel::Vec3 screenStart = transform * jadel::Vec3(start.x, start.y, 1.0f);
    jadel::Vec3 screenEnd = transform * jadel::Vec3(end.x, end.y, 1.0f);
    graphics.drawRectRelative(screenStart.x, screenStart.y, screenEnd.x, screenEnd.y, color);
}

void graphicsDrawRect(jadel::Rectf dim, uint32 color, jadel::Mat3 transform)
{
    graphicsDrawRect(dim.getPointA(), dim.getPointB(), color, transform);
}

void graphicsDrawRect(float xStart, float yStart, float xEnd, float yEnd, uint32 color, jadel::Mat3 transform)
{
    graphicsDrawRect(jadel::Vec2(xStart, yStart), jadel::Vec2(xEnd, yEnd), color, transform);
}

void graphicsDrawTexture(jadel::Rectf dim, const Texture *texture, jadel::Mat3 transform)
{
    jadel::Rectf targetRect = transform * dim;
    jadel::Rectf sourceRect(texture->texStartCoords, texture->texEndCoords);
    graphics.blitRelative(texture->surface, targetRect, sourceRect);
}

void graphicsSetFilterMode(bool filterActive)
{
    bilinearFilterMode = filterActive;
    graphics.setFlag(jadel::JADEL_GRAPHICS_BLIT_FILTER_BILINEAR, bilinearFilterMode);
}

bool graphicsGetFilterMode()
{
    return bilinearFilterMode;
}

void graphicsCreateBWSurfaceFromEqualSurface(const jadel::Surface* source, jadel::Surface* target)
{
    if (!source || !target || source->width != target->width || source->height != target->height)
    {
        return;
    }
    uint32* sourcePixels = (uint32*)source->pixels;
    uint32* targetPixels = (uint32*)target->pixels;
    for (int y = 0; y < source->height; ++y)
    {
        for (int x = 0; x < source->width; ++x)
        {

            int index = x + y * source->width;
            //targetPixels[index] = sourcePixels[index];
            
            uint32 sourcePixel = sourcePixels[index];
            uint32 sA = sourcePixel >> jadel::ALPHA_SHIFT & 0xff;
            uint32 sR = (sourcePixel >> jadel::RED_SHIFT & 0xff);
            uint32 sG = (sourcePixel >> jadel::GREEN_SHIFT & 0xff);
            uint32 sB = (sourcePixel >> jadel::BLUE_SHIFT & 0xff);
            uint32 bwPixelElement = (sR + sG + sB) / 3;

            targetPixels[index] = (sA << jadel::ALPHA_SHIFT)
            | (bwPixelElement << jadel::RED_SHIFT)
            | (bwPixelElement << jadel::GREEN_SHIFT)
            | (bwPixelElement << jadel::BLUE_SHIFT);
        }
    }
}

Texture graphicsCreateTexture(jadel::Vec2 texCoordA, jadel::Vec2 texCoordB, const jadel::Surface *surface)
{
    if (!surface)
    {
        return errorTexture;
    }
    Texture result;
    if (texCoordA.x == texCoordB.x || texCoordA.y == texCoordB.y)
    {
        ERRMSG("Invalid texCoords\n");
        return errorTexture;
    }
    float finalAX = texCoordA.x;
    float finalAY = texCoordA.y;
    float finalBX = texCoordB.x;
    float finalBY = texCoordB.y;
    bool xCoordsCorrectOrder = finalAX < finalBX;
    bool yCoordsCorrectOrder = finalAY < finalBY;
    if (!xCoordsCorrectOrder)
    {
        swapFloat(&finalAX, &finalBX);
    }
    if (!yCoordsCorrectOrder)
    {
        swapFloat(&finalAY, &finalBY);
    }

    result.surface = surface;
    result.heightInPixels = (finalBY - finalAY) * surface->height;
    result.widthInPixels = (finalBX - finalAX) * surface->width;
    result.texStartCoords = jadel::Vec2(finalAX, finalAY);
    result.texEndCoords = jadel::Vec2(finalBX, finalBY);
    return result;
}
