#pragma once

// Include this if you need platform specific windows events and types

#ifdef _WIN32
#ifdef STRICT
#undef STRICT
#endif
#define STRICT 1

// Also control related defines
#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <windowsx.h>

// Undefine asinine macros
#undef CreateSemaphore
#undef CreateMutex
#undef CreateEvent
#undef CreateFile
#undef DeleteFile
#undef MoveFile
#undef CopyFile
#undef DrawText
#undef GetObject
#undef LoadImage
#undef MessageBox

#include "PlatformKit/WindowEvents.h"

namespace PlatformKit::Win32 {
        using WindowHandle = HWND;
        using InstanceHandle = HINSTANCE;
        using MessageResult = LRESULT;
        using MessageValueUnsigned = WPARAM;
        using MessageValueSigned = LPARAM;

        enum class ResizeEdge {
            Left        = WMSZ_LEFT,
            Right       = WMSZ_RIGHT,
            Top         = WMSZ_TOP,
            TopLeft     = WMSZ_TOPLEFT,
            TopRight    = WMSZ_TOPRIGHT,
            Bottom      = WMSZ_BOTTOM,
            BottomLeft  = WMSZ_BOTTOMLEFT,
            BottomRight = WMSZ_BOTTOMRIGHT,
        };

        using Rectangle = RECT;
}
#endif