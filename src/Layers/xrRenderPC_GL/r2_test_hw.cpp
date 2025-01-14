#include "stdafx.h"

class sdl_window_test_helper
{
    SDL_Window* m_window{};
    CHW m_hw;

public:
    sdl_window_test_helper()
    {
        u32 flags{};
        m_hw.SetPrimaryAttributes(flags);
#ifndef ANDROID
        m_window = SDL_CreateWindow("TestOpenGLWindow", 0, 0, 1, 1, SDL_WINDOW_HIDDEN | flags);
        if (!m_window)
        {
            Log("~ Cannot create helper window for OpenGL test:", SDL_GetError());
            return;
        }
#endif
        m_hw.CreateDevice(m_window);
    }

    [[nodiscard]]
    bool successful() const
    {
#ifdef ANDROID
        return true;
#else
        return m_window && m_hw.m_context && m_hw.pFB;
#endif
    }

    ~sdl_window_test_helper()
    {
#ifndef ANDROID
        m_hw.DestroyDevice();
        SDL_DestroyWindow(m_window);
#endif
    }
};

BOOL xrRender_test_hw()
{
    ZoneScoped;
    // Check if minimal required OpenGL features are available
    const sdl_window_test_helper windowTest;
    if (windowTest.successful())
        return TRUE;

    return FALSE;
}
