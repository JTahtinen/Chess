#pragma once
#include "defs.h"
#include "graphics.h"

enum
{
    CLICKABLE_ALIGN_NULL = 0,
    CLICKABLE_ALIGN_CENTER = 1 << 0,
    CLICKABLE_ALIGN_LEFT = 1 << 1,
    CLICKABLE_ALIGN_RIGHT = 1 << 2,
    CLICKABLE_ALIGN_TOP = 1 << 3,
    CLICKABLE_ALIGN_BOTTOM = 1 << 4
};

enum
{
    CLICKABLE_VISUAL_DEFAULT = 0,
    CLICKABLE_VISUAL_TEXTURED = 1 << 0,
    CLICKABLE_VISUAL_COLORED = 1 << 1,
    CLICKABLE_VISUAL_HIGHLIGHT_HOVER = 1 << 2,
    CLICKABLE_VISUAL_HIGHLIGHT_CLICK = 1 << 3,
    CLICKABLE_VISUAL_INVISIBLE = 1 << 4
};

/* Clickable dimensions determine where the clickable starts and ends
    relative to the position. Values can be negative. */
struct ClickableArea
{
    jadel::Vec2 _pos;
    jadel::Rectf _dimensions;
    bool _clicked;
    bool _released;
    bool _hovered;
    bool _previouslyHovered;
    bool _held;

    void update();
    bool isClicked() const;
    bool isHovered() const;
    bool isEntered() const;
    bool isExited() const;
    bool isHeld() const;
    bool isReleased() const;

    jadel::Vec2 setPos(jadel::Vec2 pos);
    jadel::Vec2 setPos(float x, float y);
    jadel::Rectf setDimensions(jadel::Rectf dim);
    jadel::Rectf setDimensions(float xStart, float yStart, float xEnd, float yEnd);
    jadel::Rectf alignLeft();
    jadel::Rectf alignRight();
    jadel::Rectf alignTop();
    jadel::Rectf alignBottom();
    jadel::Rectf alignCenter();
    float getWidth() const;
    float getHeight() const;

    ClickableArea(jadel::Vec2 pos, jadel::Rectf dimensions);
    ClickableArea(jadel::Vec2 pos, jadel::Rectf dimensions, uint32 alignment);
    ClickableArea(jadel::Vec2 pos, float width, float height, uint32 alignment);
    ClickableArea(jadel::Vec2 pos, float width, float height);
    ClickableArea() = default;
};

/*
    If flags inculude both CLICKABLE_TEXTURED and CLICKABLE_COLORED,
    a colored rectangle will be drawn with the texture on top of it.
    If only idleTexture is determined, it will also be used as
    hoverTexture and clickTexture. If flags contain CLICKABLE_VISUAL_DEFAULT,
    (same as NULL), default colors will be used.
*/
struct ClickableVisuals
{
    uint32 flags;
    const Texture *idleTexture;
    uint32 idleColor;
    uint32 hoverColor;
    uint32 clickColor;
};

struct Clickable
{
    void (*_callback)();
    bool _colorSet;
    bool _textureSet;
    ClickableArea _area;
    ClickableVisuals _visuals;
    const Texture *_currentTexture;
    uint32 _currentColor;

    void update();
    void render();

    bool isClicked() const;
    bool isHovered() const;
    bool isEntered() const;
    bool isExited() const;
    bool isHeld() const;
    bool isReleased() const;

    void setDimensions(jadel::Rectf dimensions);
    void setDimensions(float xStart, float yStart, float xEnd, float yEnd);
    void setPos(jadel::Vec2 pos);
    void setPos(float x, float y);
    void setArea(jadel::Vec2 pos, jadel::Rectf dimensions);
    void setArea(jadel::Vec2 pos, float xStart, float yStart, float xEnd, float yEnd);
    void setArea(float posX, float posY, jadel::Rectf dim);
    void setArea(float posX, float posY, float xStart, float yStart, float xEnd, float yEnd);
    void setTexture(const Texture *texture);
    void setColor(uint32 color);
    void setColor(jadel::Color color);
    void setColor(float a, float r, float g, float b);
    void setHighlightColor(float r, float g, float b);
    void addVisualFlags(uint32 flags);
    void copyVisualsFrom(const Clickable *other);
    void setCallBack(void (*callback)());
    Clickable(uint32 visualFlags);
    Clickable();
};

void initClickables();