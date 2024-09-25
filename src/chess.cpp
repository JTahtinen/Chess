#include <jadel.h>
#include <thread>
#include "game.h"
#include "application.h"
#include "graphics.h"
#include "gamerender.h"
#include "render.h"
#include "render.h"
#include "clickable.h"
#include "timer.h"

static float frameTime = 0;
static int framesPerSecond = 0;

static void updateFPS()
{
    jadel::message("FPS: %d\n", framesPerSecond);
}

void tick()
{
    updateApplication(frameTime);
    updateGame();
}

void init()
{
    graphicsInit(1280, 720);
    renderInit();
    gameRenderInit();
    initClickables();
    initGame();
    startGame();
    gameRender();
    graphicsUpdate();
    JadelUpdate();
    tick();
}

int JadelMain(int argc, char **argv)
{
    JadelInit(MB(50));
    if (!JadelIsInitialized())
    {
        return -1;
    }

    jadel::allocateConsole();
    jadel::message("Hello, Chess!\n\n");

    Timer frameTimer;
    uint32 elapsedInMillis = 0;
    uint32 minFrameTime = 1000 / 165;
    uint32 accumulator = 0;
    init();
    int frames = 0;
    frameTimer.start();
    while (true)
    {
        JadelUpdate();
        gameRender();
        ++frames;
        graphicsUpdate();
        elapsedInMillis = frameTimer.getMillisSinceLastUpdate();
        accumulator += elapsedInMillis;

        if (accumulator >= 1000)
        {
            accumulator %= 1000;
            framesPerSecond = frames;
            frames = 0;
            updateFPS();
        }

        if (elapsedInMillis < minFrameTime)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(minFrameTime - elapsedInMillis));
        }
        if (elapsedInMillis > 0)
        {
            frameTime = (float)frameTimer.getMillisSinceLastUpdate() * 0.001f;
        }
        tick();
        uint32 debugTime = frameTimer.getMillisSinceLastUpdate();
        // jadel::message("%f\n", frameTime);

        frameTimer.update();
    }
    return 0;
}