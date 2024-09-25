#pragma once
#include <jadel.h>
#include "graphics.h"

extern jadel::Mat3 projMatrix;

void renderSubmitWorldRect(jadel::Rectf rect, jadel::Vec2 pos, uint32 color);

void renderSubmitWorldTexture(jadel::Rectf rect, jadel::Vec2 pos, const Texture *texture);

void renderSubmitScreenRect(jadel::Rectf rect, jadel::Vec2 pos, uint32 color);

void renderSubmitScreenTexture(jadel::Rectf rect, jadel::Vec2 pos, const Texture *texture);

void renderSubmitRectRenderable(jadel::Rectf dim, uint32 color, jadel::Mat3 transform);

void renderSubmitRectRenderable(jadel::Vec2 start, jadel::Vec2 end, uint32 color, jadel::Mat3 transform);

void renderSubmitRectRenderable(float xStart, float yStart, float xEnd, float yEnd, uint32 color, jadel::Mat3 transform);

void renderSubmitTextureRenderable(jadel::Rectf dim, const Texture *texture, jadel::Mat3 transform);

void renderSubmitTextureRenderable(jadel::Vec2 start, jadel::Vec2 end, const Texture* texture, jadel::Mat3 transform);

void renderSubmitTextureRenderable(float xStart, float yStart, float xEnd, float yEnd, const Texture* texture, jadel::Mat3 transform);

void renderFlush();

void renderClear();

void renderInit();

