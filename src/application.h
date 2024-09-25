#pragma once
#include <jadel.h>

float getFrameTime();

jadel::Vec2 getMousePos();

bool isMouseLeftClicked();

bool isMouseLeftReleased();

void updateApplication(float frameTime);