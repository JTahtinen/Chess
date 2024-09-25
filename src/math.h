#pragma once
#include <jadel.h>

inline jadel::Vec2 getRectDimensions(jadel::Rectf rect)
{
    jadel::Vec2 result(jadel::absFloat(rect.x1 - rect.x0), jadel::absFloat(rect.y1 - rect.y0));
    return result;
}

inline jadel::Mat3 getTranslationMatrix(jadel::Vec2 translation)
{
    jadel::Mat3 result(1.0f, 0.0f, 0.0f,
                       0.0f, 1.0f, 0.0f,
                       translation.x, translation.y, 1.0f);
    return result;
}