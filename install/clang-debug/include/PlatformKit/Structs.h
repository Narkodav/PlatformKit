#pragma once
#include <cstdint>

namespace PlatformKit {
    enum class CursorMode {
        Normal,
        Hidden,
        Disabled  // For raw motion input
    };

    struct Position
    {
        int32_t x;
        int32_t y;
    };

    struct Extent
    {
        int32_t width, height;
    };

    struct WindowAttributes
    {
        enum class Type {
            // Window behavior
            Resizable,                  // bool
            Visible,                    // bool
            Decorated,                  // bool
            Focused,                    // bool
            AutoIconify,                // bool
            Floating,                   // bool
            Maximized,                  // bool
            CenterCursor,               // bool
            TransparentFramebuffer,     // bool
            FocusOnShow,                // bool

            // Input mode
            CursorMode,                 // CursorMode enum
        };

        // Window behavior
        bool resizable = true;
        bool visible = true;
        bool decorated = true;
        bool focused = true;
        bool autoIconify = true;
        bool floating = false;
        bool maximized = false;
        bool centerCursor = false;
        bool transparentFramebuffer = false;
        bool focusOnShow = true;

        CursorMode cursorMode = CursorMode::Normal;

        static WindowAttributes defaultAtr() {
            return WindowAttributes();
        }

        static WindowAttributes firstPersonGameMaximisedAtr() {
            WindowAttributes atr;
            atr.centerCursor = true;
            atr.cursorMode = CursorMode::Disabled;
            atr.maximized = true;
            return atr;
        }

        static WindowAttributes firstPersonGameMinimisedAtr() {
            WindowAttributes atr;
            atr.centerCursor = true;
            atr.cursorMode = CursorMode::Disabled;
            return atr;
        }
    };
}