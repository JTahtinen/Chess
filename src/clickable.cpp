#include "defs.h"
#include "clickable.h"
#include "application.h"
#include "util.h"
#include "render.h"

static ClickableVisuals defaultVisuals;

void initClickables()
{
    defaultVisuals = {};
    defaultVisuals.flags = CLICKABLE_VISUAL_DEFAULT;
    defaultVisuals.idleColor = jadel::Color(1, 0.4f, 0.4f, 0.1f).toU32();
    defaultVisuals.hoverColor = jadel::Color(0.2f, 1.0f, 1.0f, 1.0f).toU32();
    defaultVisuals.clickColor = jadel::Color(0.4f, 1.0f, 1.0f, 1.0f).toU32();
}

static bool isMouseWithinClickableArea(const ClickableArea *clickable)
{

    jadel::Rectf projectedRect = {projMatrix * clickable->_dimensions.getPointA(), projMatrix * clickable->_dimensions.getPointB()};
    jadel::Vec2 mousePos = getMousePos();
    bool xWithin = mousePos.x >= clickable->_pos.x + projectedRect.x0 && mousePos.x < clickable->_pos.x + projectedRect.x1;
    bool yWithin = mousePos.y >= clickable->_pos.y + projectedRect.y0 && mousePos.y < clickable->_pos.y + projectedRect.y1;
    bool result = (xWithin && yWithin);
    return result;
}

void ClickableArea::update()
{
    // Click and release cannot be true in two subsequent updates
    this->_clicked = false;
    this->_released = false;
    this->_previouslyHovered = this->_hovered;
    this->_hovered = isMouseWithinClickableArea(this);

    if (this->_hovered)
    {
        this->_clicked = isMouseLeftClicked();
        if (this->_clicked)
        {
            this->_held = true;
            this->_released = false;
        }
        else if (this->_held)
        {
            this->_released = isMouseLeftReleased();
            if (this->_released)
            {
                this->_held = false;
            }
        }
    }
    else if (this->_held && isMouseLeftReleased())
    {
        this->_held = false;
    }
}

bool ClickableArea::isClicked() const
{
    return this->_clicked;
}

bool ClickableArea::isHovered() const
{
    return this->_hovered;
}

bool ClickableArea::isEntered() const
{
    bool result = (this->_hovered && !this->_previouslyHovered);
    return result;
}

bool ClickableArea::isExited() const
{
    bool result = (!this->_hovered && this->_previouslyHovered);
    return result;
}

bool ClickableArea::isHeld() const
{
    return this->_held;
}

bool ClickableArea::isReleased() const
{
    return this->_released;
}

ClickableArea::ClickableArea(jadel::Vec2 pos, jadel::Rectf dimensions)
    : _pos(setPos(pos)), _dimensions(setDimensions(dimensions)), _clicked(false), _released(false), _hovered(false), _previouslyHovered(false), _held(false)
{
}

ClickableArea::ClickableArea(jadel::Vec2 pos, jadel::Rectf dimensions, uint32 alignment)
    : ClickableArea(pos, dimensions)
{
    if (alignment & CLICKABLE_ALIGN_CENTER)
    {
        this->alignCenter();
        return;
    }
    if (alignment & CLICKABLE_ALIGN_LEFT)
    {
        this->alignLeft();
    }
    else if (alignment & CLICKABLE_ALIGN_RIGHT)
    {
        this->alignRight();
    }
    if (alignment & CLICKABLE_ALIGN_TOP)
    {
        this->alignTop();
    }
    else if (alignment & CLICKABLE_ALIGN_BOTTOM)
    {
        this->alignBottom();
    }
}

ClickableArea::ClickableArea(jadel::Vec2 pos, float width, float height, uint32 alignment)
    : ClickableArea(pos, jadel::Rectf(0, 0, width, height), alignment)
{
}

ClickableArea::ClickableArea(jadel::Vec2 pos, float width, float height)
    : ClickableArea(pos, width, height, CLICKABLE_ALIGN_NULL)
{
}

jadel::Vec2 ClickableArea::setPos(jadel::Vec2 pos)
{
    this->_pos = pos;
    return this->_pos;
}

jadel::Vec2 ClickableArea::setPos(float x, float y)
{
    return setPos(jadel::Vec2(x, y));
}

jadel::Rectf ClickableArea::setDimensions(jadel::Rectf dim)
{
    if (dim.x0 == dim.x1)
    {
        dim.x0 = -0.05f;
        dim.x1 = 0.05f;
    }
    if (dim.y0 == dim.y1)
    {
        dim.y0 = -0.05f;
        dim.y1 = 0.05f;
    }
    swapFloatIfHigherToLower(&dim.x0, &dim.x1);
    swapFloatIfHigherToLower(&dim.y0, &dim.y1);
    this->_dimensions = dim;
    return this->_dimensions;
}

jadel::Rectf ClickableArea::setDimensions(float xStart, float yStart, float xEnd, float yEnd)
{
    return setDimensions(jadel::Rectf(xStart, yStart, xEnd, yEnd));
}

jadel::Rectf ClickableArea::alignLeft()
{
    return setDimensions(0, _dimensions.y0, this->getWidth(), _dimensions.y1);
}

jadel::Rectf ClickableArea::alignRight()
{
    return setDimensions(-this->getWidth(), _dimensions.y0, 0, _dimensions.y1);
}

jadel::Rectf ClickableArea::alignTop()
{
    return setDimensions(_dimensions.x0, -this->getHeight(), _dimensions.x1, 0);
}

jadel::Rectf ClickableArea::alignBottom()
{
    return setDimensions(_dimensions.x0, 0, _dimensions.x1, this->getHeight());
}

jadel::Rectf ClickableArea::alignCenter()
{
    float halfWidth = this->getWidth() * 0.5f;
    float halfHeight = this->getHeight() * 0.5f;
    return setDimensions(-halfWidth, -halfHeight, halfWidth, halfHeight);
}

float ClickableArea::getWidth() const
{
    float result = this->_dimensions.x1 - this->_dimensions.x0;
    return result;
}

float ClickableArea::getHeight() const
{
    float result = this->_dimensions.y1 - this->_dimensions.y0;
    return result;
}

enum
{
    CLICKABLE_IDLE = 0,
    CLICKABLE_EXITED,
    CLICKABLE_RELEASED,
    CLICKABLE_CLICKED,
    CLICKABLE_HELD_OUTSIDE,
    CLICKABLE_HELD,
    CLICKABLE_HOVERED,
    CLICKABLE_ENTERED,
    NUM_CLICKABLE_STATES
};

void Clickable::update()
{
    this->_area.update();
    uint32 visualState = CLICKABLE_IDLE;

    // Ordering is important, as each visual state
    // takes precedence to the next.
    if (this->isExited())
    {
        visualState = CLICKABLE_EXITED;
    }
    else if (this->isReleased())
    {
        visualState = CLICKABLE_RELEASED;
        if (_callback)
        {
            _callback();
        }
    }
    else if (this->isClicked())
    {
        visualState = CLICKABLE_CLICKED;
    }
    else if (this->isHovered())
    {
        if (this->isHeld())
        {
            visualState = CLICKABLE_HELD;
        }
        else
        {
            visualState = CLICKABLE_HOVERED;
        }
    }
    else if (this->isEntered())
    {
        visualState = CLICKABLE_ENTERED;
    }
    else
    {
        visualState = CLICKABLE_IDLE;
    }
    /*
    bool textured = this->_visuals.flags & CLICKABLE_VISUAL_TEXTURED;
    bool colored = this->_visuals.flags & CLICKABLE_VISUAL_COLORED;
    
        switch (visualState)
        {
        case CLICKABLE_IDLE:
        case CLICKABLE_EXITED:
        {
            if (colored)
            {
                this->_currentColor = this->_visuals.idleColor;
            }
            if (textured)
            {
                this->_currentTexture = this->_visuals.idleTexture;
            }
            break;
        }
        case CLICKABLE_CLICKED:
        case CLICKABLE_HELD:
        {
            if (colored)
            {
                this->_currentColor = this->_visuals.clickColor;
            }
            if (textured)
            {
                this->_currentTexture = this->_visuals.idleTexture;
            }
            break;
        }
        case CLICKABLE_ENTERED:
        case CLICKABLE_HOVERED:
        case CLICKABLE_RELEASED:
        {
            if (colored)
            {
                this->_currentColor = this->_visuals.hoverColor;
            }
            if (textured)
            {
                this->_currentTexture = this->_visuals.idleTexture;
            }
            break;
        }
        }*/
}

void Clickable::render()
{
    uint32 flags = this->_visuals.flags;
    if ((flags & CLICKABLE_VISUAL_INVISIBLE))
    {
        return;
    }
    const Texture *textureToRender;
    bool highlightClick = flags & CLICKABLE_VISUAL_HIGHLIGHT_CLICK;
    bool highlightHover = flags & CLICKABLE_VISUAL_HIGHLIGHT_HOVER;
    if (flags & CLICKABLE_VISUAL_COLORED)
    {
        uint32 idleColorToRender;
        uint32 hoverColorToRender;
        uint32 clickColorToRender;
        if (!_colorSet)
        {
            idleColorToRender = defaultVisuals.idleColor;
            hoverColorToRender = defaultVisuals.hoverColor;
            clickColorToRender = defaultVisuals.clickColor;
        }
        else
        {
            idleColorToRender = this->_visuals.idleColor;
            hoverColorToRender = this->_visuals.hoverColor;
            clickColorToRender = this->_visuals.clickColor;
        }
        //    DEBUGMSG("Rendering clickable color\n");
        renderSubmitScreenRect(this->_area._dimensions, this->_area._pos, idleColorToRender);
        if (highlightClick && this->isHeld())
        {
            renderSubmitScreenRect(this->_area._dimensions, this->_area._pos, clickColorToRender);
        }
        else if (highlightHover && this->isHovered())
        {
            renderSubmitScreenRect(this->_area._dimensions, this->_area._pos, hoverColorToRender);
        }
    }
    if (flags & CLICKABLE_VISUAL_TEXTURED)
    {
        if (_textureSet)
        {
            //     DEBUGMSG("Rendering clickable tex\n");
            renderSubmitScreenTexture(this->_area._dimensions, this->_area._pos, this->_visuals.idleTexture);
        }
    }
}

bool Clickable::isClicked() const
{
    return this->_area.isClicked();
}

bool Clickable::isHovered() const
{
    return this->_area.isHovered();
}

bool Clickable::isEntered() const
{
    return this->_area.isEntered();
}
bool Clickable::isExited() const
{
    return this->_area.isExited();
}

bool Clickable::isHeld() const
{
    return this->_area.isHeld();
}

bool Clickable::isReleased() const
{
    return this->_area.isReleased();
}

void Clickable::setDimensions(jadel::Rectf dimensions)
{
    this->_area.setDimensions(dimensions);
}

void Clickable::setDimensions(float xStart, float yStart, float xEnd, float yEnd)
{
    this->setDimensions(jadel::Rectf(xStart, yStart, xEnd, yEnd));
}

void Clickable::setPos(jadel::Vec2 pos)
{
    this->_area.setPos(pos);
}

void Clickable::setPos(float x, float y)
{
    this->setPos(jadel::Vec2(x, y));
}
void Clickable::setArea(jadel::Vec2 pos, jadel::Rectf dimensions)
{
    this->setPos(pos);
    this->setDimensions(dimensions);
}

void Clickable::setArea(jadel::Vec2 pos, float xStart, float yStart, float xEnd, float yEnd)
{
    this->setPos(pos);
    this->setDimensions(jadel::Rectf(xStart, yStart, xEnd, yEnd));
}

void Clickable::setArea(float posX, float posY, jadel::Rectf dimensions)
{
    this->setPos(jadel::Vec2(posX, posY));
    this->setDimensions(dimensions);
}

void Clickable::setArea(float posX, float posY, float xStart, float yStart, float xEnd, float yEnd)
{
    this->setPos(posX, posY);
    this->setDimensions(xStart, yStart, xEnd, yEnd);
}

void Clickable::setTexture(const Texture *texture)
{
    if (!texture)
    {
        ERRMSG("Tried to set NULL texture for Clickable\n");
        return;
    }
    this->_visuals.idleTexture = texture;
    this->_textureSet = true;
}

void Clickable::setColor(uint32 color)
{
    this->_visuals.idleColor = color;
    this->_colorSet = true;
}

void Clickable::setColor(jadel::Color color)
{
    this->setColor(color.toU32());
}

void Clickable::setColor(float a, float r, float g, float b)
{
    this->setColor(jadel::Color(a, r, g, b));
}

void Clickable::setHighlightColor(float r, float g, float b)
{
    this->_visuals.hoverColor = jadel::Color(0.3f, r, g, b).toU32();
}

void Clickable::addVisualFlags(uint32 flags)
{
    this->_visuals.flags |= flags;
}

void Clickable::copyVisualsFrom(const Clickable *other)
{
    if (!other)
    {
        ERRMSG("Tried to copy visuals from NULL Clickable\n");
        return;
    }
    this->_visuals = other->_visuals;
}

void Clickable::setCallBack(void (*callback)())
{
    this->_callback = callback;
}

Clickable::Clickable(uint32 visualFlags)
    : _callback(NULL), _colorSet(false) , _textureSet(false)
{
    this->_visuals.flags = visualFlags;
    if (visualFlags == CLICKABLE_VISUAL_DEFAULT)
    {
        addVisualFlags(CLICKABLE_VISUAL_COLORED 
        | CLICKABLE_VISUAL_HIGHLIGHT_HOVER 
        | CLICKABLE_VISUAL_HIGHLIGHT_CLICK);
    }
}

Clickable::Clickable()
    : Clickable(CLICKABLE_VISUAL_DEFAULT)
{
}
