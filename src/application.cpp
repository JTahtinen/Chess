#include "application.h"

static jadel::Vec2 _mousePos;
static bool _mouseLeftClicked = false;
static bool _mouseLeftReleased = false;
static float _frameTime = 0;

float getFrameTime()
{
    return _frameTime;
}

jadel::Vec2 getMousePos()
{
    return _mousePos;
}

bool isMouseLeftClicked()
{
    return _mouseLeftClicked;
}

bool isMouseLeftReleased()
{
    return _mouseLeftReleased;
}

void updateApplication(float frameTime)
{
    _frameTime = frameTime;
    _mousePos = jadel::Vec2(jadel::inputGetMouseXRelative(), jadel::inputGetMouseYRelative());
    _mouseLeftClicked = jadel::inputIsMouseLeftClicked();
    _mouseLeftReleased = jadel::inputIsMouseLeftReleased();
}