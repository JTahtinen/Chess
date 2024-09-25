#pragma once
#include <jadel.h>

struct Texture
{
    const jadel::Surface* surface;
    int widthInPixels;
    int heightInPixels;
    jadel::Vec2 texStartCoords;
    jadel::Vec2 texEndCoords;
};

extern jadel::Surface winSurface;

extern jadel::Surface errorSurface;

extern Texture errorTexture;

extern float aspect;

extern bool bilinearFilterMode;

void graphicsDrawRect(jadel::Vec2 start, jadel::Vec2 end, uint32 color, jadel::Mat3 transform);

void graphicsDrawRect(jadel::Rectf dim, uint32 color, jadel::Mat3 transform);

void graphicsDrawRect(float xStart, float yStart, float xEnd, float yEnd, uint32 color, jadel::Mat3 transform);

void graphicsDrawTexture(jadel::Rectf dim, const Texture *texture, jadel::Mat3 transform);

void graphicsInit(int clientWidth, int clientHeight);

void graphicsUpdate();

void graphicsClear();

void graphicsSetFilterMode(bool filterActive);

bool graphicsGetFilterMode();

void graphicsCreateBWSurfaceFromEqualSurface(const jadel::Surface* source, jadel::Surface* target);

Texture graphicsCreateTexture(jadel::Vec2 texCoordA, jadel::Vec2 texCoordB, const jadel::Surface *surface);
